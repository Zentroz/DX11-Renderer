#include"common.hlsl"

cbuffer ScreenOutput : register(b3) 
{
    int outputTextureIndex;
    int3 paddings;
}

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

Texture2D outputTex : register(t0);
Texture2D albedoTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D materialTex : register(t3);
Texture2D depthTex : register(t4);
Texture2D backgroundTex : register(t5);
SamplerState samp : register(s0);
SamplerState samp1 : register(s1);
SamplerState samp2 : register(s2);
SamplerState samp3 : register(s3);
SamplerState samp4 : register(s4);
SamplerState samp5 : register(s5);

float4 DepthToScreenColor(float depth, float nearPlane, float farPlane)
{
	float z = nearPlane * farPlane / (farPlane - depth * (farPlane - nearPlane));
	z /= farPlane;
	z = 1.0 - z;
	return z;
}

float Linear01Depth(float depth, float nearPlane, float farPlane)
{
	return nearPlane / (farPlane - depth * (farPlane - nearPlane));
}

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
    float2 ScreenSize = float2(1536, 793);
    float2 uv = pos.xy / ScreenSize;
    // float2 uv = pos.xy / ScreenSize;
    float4 output = 0;
    
	float nearPlane = 0.01;
	float farPlane = 100.0;
    
	float depth = depthTex.Sample(samp4, uv).r;

	if (Linear01Depth(depth, nearPlane, farPlane)  >= 0.999)
	{
		return backgroundTex.Sample(samp5, uv);
	}

    if (outputTextureIndex == 0) {
        output = outputTex.Sample(samp, uv);
    }
    else if (outputTextureIndex == 1) {
        output = albedoTex.Sample(samp1, uv);
    }
    else if (outputTextureIndex == 2) {
        output = normalTex.Sample(samp2, uv);
    }
    else if (outputTextureIndex == 3) {
        output = materialTex.Sample(samp3, uv);
    }
    else {
		output = DepthToScreenColor(depth, nearPlane, farPlane);
	}

    return output;
}