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

float DepthToScreen(float depth)
{
    float z = LinearizeDepth(depth, 0.1, 20);
    z /= 20.0f;
    z = 1.0 - z;
    return z;
}

float4 ClipSpaceFromDepth(float2 screenUV, float z)
{
    return float4(screenUV.x * 2.0f - 1.0f, -(screenUV.y * 2.0f - 1.0f), z, 1.0f);
}

float3 PositionFromDepth(float2 screenUV, float rawDepth)
{
    float4 clipSpacePos = ClipSpaceFromDepth(screenUV, rawDepth);
    
    float4 worldPos = mul(clipSpacePos, invViewProj);
    
    worldPos.xyz /= worldPos.w;
    
    return worldPos.xyz;
}

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
    float2 ScreenSize = float2(1536, 793);
    float2 uv = pos.xy / ScreenSize;
    
    float4 albedoSample = albedoTex.Sample(samp, uv);
    float4 normalSample = normalTex.Sample(samp1, uv);
    float4 materialSample = materialTex.Sample(samp2, uv);
    float shadowDepth = shadowDepthTex.Sample(samp4, uv).r;
    float depth = depthTex.Sample(samp3, uv).r;
    
    float roughness = max(materialSample.r, 0.15);
    float metallic = materialSample.g;
    
    float3 worldPosition = PositionFromDepth(uv, depth);

    float3 N = normalSample.xyz * 2.0f - 1.0f;;
    float3 L = normalize(mainLightDirection.xyz);
    float3 V = normalize(cameraPosition.xyz - worldPosition);
    
    float3 color = CookTorranceBRDF(albedoSample.xyz, metallic, roughness, N, V, L, mainLightColor.rgb);
    // float3 color = CalculatePhongLighting(N, worldPosition, cameraPosition.xyz, mainLightDirection.xyz, mainLightColor.rgb, float3(0, 0, 0), 0);
    float4 result = float4(color, 1);

    return result;
}