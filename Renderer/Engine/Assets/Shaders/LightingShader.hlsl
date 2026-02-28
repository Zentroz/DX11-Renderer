#include"common.hlsl"
#include"lighting.hlsl"

float4 VSMain(uint id : SV_VertexID) : SV_Position
{
    float2 pos[3] =
    {
        float2(-1, -1),
        float2(-1, 3),
        float2(3, -1)
    };
    
    return float4(pos[id], 0, 1);
}

cbuffer InverseVPMatrix : register(b2)
{
    matrix invViewProj;
}

cbuffer LightPassData : register(b3)
{
	Light lights[8];
	int4 lightCount;
	float4 shadowBias;
	int4 pcFilterSampleCount;
}

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D materialTex : register(t2);
Texture2D depthTex : register(t3);
Texture2D shadowDepthTex : register(t4);
SamplerState samp : register(s0);
SamplerState samp1 : register(s1);
SamplerState samp2 : register(s2);
SamplerState samp3 : register(s3);
SamplerState samp4 : register(s4);

float LinearizeDepth(float z, float near, float far)
{
    return near * far / (far - z * (far - near));
}

float DepthToScreen(float depth, float nearPlane, float farPlane)
{
    float z = LinearizeDepth(depth, nearPlane, farPlane);
    z /= farPlane;
    z = 1.0 - z;
    return z;
}

float4 ClipSpaceFromDepth(float2 screenUV, float z)
{
    return float4(screenUV.x * 2.0f - 1.0f, -(screenUV.y * 2.0f - 1.0f), z, 1.0f);
}

float3 PositionFromDepth(float2 screenUV, float rawDepth, matrix inverseViewProj)
{
    float4 clipSpacePos = ClipSpaceFromDepth(screenUV, rawDepth);
    
	float4 worldPos = mul(clipSpacePos, inverseViewProj);
    
    worldPos.xyz /= worldPos.w;
    
    return worldPos.xyz;
}

float3 ACES(float3 x)
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
	float2 ScreenSize = float2(timeAndScreen.z, timeAndScreen.w);
	float2 uv = pos.xy / ScreenSize;
    
	// G-Buffers sampling
	float4 albedoSample = albedoTex.Sample(samp, uv);
	float4 normalSample = normalTex.Sample(samp1, uv);
	float4 materialSample = materialTex.Sample(samp2, uv);
	float depth = depthTex.Sample(samp3, uv).r;
    
	// ORM Texture properties
	float ao = materialSample.r;
	float roughness = materialSample.g;
	float metallic = materialSample.b;
    
	float3 worldPosition = PositionFromDepth(uv, depth, invViewProj);
	float3 N = normalSample.xyz;
	float3 V = normalize(cameraPosition.xyz - worldPosition);

	// Calcuation Shadows
	float4 lightSpacePos = mul(float4(worldPosition, 1), lights[0].lightVPMatrix);
	float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords.xy = projCoords.xy * 0.5f + 0.5f;
	projCoords.y = 1.0f - projCoords.y;
    
	float shadowDepth = shadowDepthTex.Sample(samp4, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = shadowBias.x;
	float shadow = (currentDepth - bias) > shadowDepth ? 0.0 : 1.0;
    
	// If outside of Projection/ShadowMap then : No Shadows
	if (projCoords.x < 0 || projCoords.x > 1 ||
    projCoords.y < 0 || projCoords.y > 1 ||
    projCoords.z < 0 || projCoords.z > 1)
	{
		shadow = 1.0;
	}
    
	// PCF Filtering on shadows
	float2 texelSize = 1.0 / 4096;
	//int range = pcFilterSampleCount.x;
	int range = 4;
        
	for (int x = -range; x <= range; ++x)
	{
		for (int y = -range; y <= range; ++y)
		{
			float2 offset = float2(x, y) * texelSize;
			float depthSample = shadowDepthTex.Sample(samp4, projCoords.xy + offset).r;
			shadow += (currentDepth - bias) > depthSample ? 0.0 : 1.0;
		}
	}
        
	shadow /= (2 * range + 1) * (2 * range + 1);
    
	// Calculation Lighting    
	float3 Lo = 0;
    
	for (int i = 0; i < lightCount.x; i++)
	{
		float3 L = -GetLightDirection(lights[i], worldPosition);
		float lightIntensity = CalculateLightIntensity(lights[i], worldPosition);
		
		// Only shadows for directional light is supported
		if (lights[i].type.x == 0)
		{
			lightIntensity *= shadow;
		}
		
		Lo += CookTorranceBRDF(albedoSample.xyz, metallic, roughness, N, V, L, lightIntensity, mainLightColor.rgb);
	}

	float3 ambient = 0.03 * albedoSample.xyz;
	float3 finalColor = ambient + Lo * ao;
    
	// Post-Processing
	finalColor = ACES(finalColor);
	finalColor = GammaCorrection(finalColor, 2.2);

	return float4(finalColor, 1.0);
}