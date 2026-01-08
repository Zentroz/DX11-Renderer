#include"common.hlsl"
#include"lighting.hlsl"

cbuffer MaterialData : register(b2)
{
    float4 diffuseColor;
    float roughness;
    float metallic;
    float2 padding;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_Position;
	float3 normalWS : NORMAL;
    float3 tangentWS : TANGENT;
    float3 bitangentWS : BINORMAL;
    float2 uv : TEXCOORD;
	float4 fragPos : FRAG_Position;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    o.fragPos = mul(float4(input.pos, 1), modelMatrix);
    o.pos = mul(o.fragPos, vpMatrix);
	o.uv = input.uv;

	// Normal
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);

    o.normalWS = mul(N, (float3x3)modelMatrix);
    o.tangentWS = mul(T, (float3x3)modelMatrix);
    o.bitangentWS = cross(o.normalWS, o.tangentWS);

    return o;
}

Texture2D diffuseTex : register(t0);
SamplerState diffSampler : register(s0);

Texture2D normalMap : register(t1);
SamplerState normalSampler : register(s1);

bool IsColorZero(float3 color)
{
	return (color.x == 0 && color.y == 0 && color.z == 0);
}
bool IsColorZero(float4 color)
{
	return (color.x == 0 && color.y == 0 && color.z == 0 && color.w == 0);
}

float4 PSMain(VSOutput input) : SV_Target
{
	// Texture Sample
	float4 diffuseSample = diffuseTex.Sample(diffSampler, input.uv);
	float4 normalSample = normalMap.Sample(normalSampler, input.uv);

	// Properties
	float3 diffuse = diffuseColor;
	float3 normalWS = normalize(input.normalWS);
	float ao = 1;

	if (!IsColorZero(diffuseSample)) diffuse = diffuse * diffuseSample.rgb;
	if (!IsColorZero(normalSample)) 
	{
		float3 normalTS = normalSample.xyz * 2.0f - 1.0f;

		// TBN
		float3 N = normalWS;
        float3 T = normalize(input.tangentWS);
        T = normalize(T - dot(T, N) * N);
        float3 B = cross(N, T);

        float3x3 TBN = float3x3(T, B, N);

        // Transform to world space
        normalWS = normalize(mul(normalTS, TBN));
	}
    
	float3 viewDir = normalize(cameraPosition - input.fragPos).xyz;
    float3 color = CookTorranceBRDF(diffuse, metallic, roughness, normalWS, viewDir, normalize(mainLightDirection.xyz), mainLightColor.rgb);

    float gamma = 1.5;
    float3 gammaCorrected = GammaCorrection(color, gamma);
    
    return float4(gammaCorrected, 1);
}