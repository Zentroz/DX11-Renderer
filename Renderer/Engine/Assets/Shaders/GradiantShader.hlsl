#include"common.hlsl"

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

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
	float2 ScreenSize = float2(1536, 793);
	float2 uv = pos.xy / ScreenSize;
	
	float3 topColor = float3(0, 0.32, 0.62);
	float3 bottomColor = float3(0.1, 0.18, 0.2);

	float t = clamp(uv.y, 0.0, 1.0);
	float4 output = float4(lerp(topColor, bottomColor, t), 1.0);
	
	return output;
}