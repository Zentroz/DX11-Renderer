#include"common.hlsl"

cbuffer PassData : register(b3)
{
	uint outlineEntity;
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
SamplerState samp : register(s0);

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
	return float4(0, 0, 0, 1);
}