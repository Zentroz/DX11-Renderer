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

Texture2D outputTex : register(t0);
SamplerState samp : register(s0);

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
    float2 ScreenSize = float2(1536, 793);
    float2 uv = pos.xy / ScreenSize;
    // float2 uv = pos.xy / ScreenSize;
    float4 output = outputTex.Sample(samp, uv);
    return output;
}