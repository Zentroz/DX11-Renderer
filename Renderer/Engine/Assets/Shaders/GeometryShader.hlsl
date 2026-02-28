#include"common.hlsl"

cbuffer MaterialData : register(b2)
{
    float4 diffuseColor;
    float roughnessMultiplier;
    float metallicMultipler;
    float aplhaCutoff;
    float padding;
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

    o.normalWS = mul(N, (float3x3) modelMatrix);
    o.tangentWS = mul(T, (float3x3) modelMatrix);
    o.bitangentWS = cross(o.normalWS, o.tangentWS);

    return o;
}

Texture2D diffuseTex : register(t0);
SamplerState diffSampler : register(s0);

Texture2D normalMap : register(t1);
SamplerState normalSampler : register(s1);

Texture2D ormMap : register(t2);
SamplerState ormSampler : register(s2);

struct GBufferOut
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 material : SV_Target2;
};

GBufferOut PSMain(VSOutput input)
{
    GBufferOut o;

	// Texture Sample
    float4 diffuseSample = diffuseTex.Sample(diffSampler, input.uv);

	if (diffuseSample.a < aplhaCutoff)
	{
		discard;
	}

	float4 normalSample = normalMap.Sample(normalSampler, input.uv);

	// Properties
	float3 diffuse = diffuseColor.rgb * diffuseSample.rgb;
	float ao = 1;
    

    // Normal Mapping
	normalSample = float4(0.5, 0.5, 1.0, 0.0);
	
	float3 normalTS = normalSample.xyz * 2.0f - 1.0f;
	float3 normalWS = input.normalWS;
    
    // TBN
	float3 N = normalWS;
	float3 T = normalize(input.tangentWS);
	T = normalize(T - dot(T, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

    // Transform to world space
	normalWS = normalize(mul(normalTS, TBN));
    
    //float3 encodedNormals = normalWS * 0.5f + 0.5f;
    
    // Writing G-Buffers
	o.albedo = float4(GammaCorrection(diffuse, 1.1), 1);
	o.normal = float4(normalWS, 1);
	float4 material = ormMap.Sample(ormSampler, input.uv);
	material.r = 1;
	material.g = material.g * roughnessMultiplier;
	material.b = material.b * metallicMultipler;

	o.material = material;
    
	return o;
}