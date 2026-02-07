#include"common.hlsl"

cbuffer ShadowPassData : register(b3) {
    matrix invView;
    matrix invProj;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

float4 VSMain(VSInput i) : SV_Position
{
    float4 wPos = mul(float4(i.pos, 1), modelMatrix);
    wPos = mul(wPos, vpMatrix);
    
    return wPos;
}

void PSMain(float4 pos : SV_Position) {}