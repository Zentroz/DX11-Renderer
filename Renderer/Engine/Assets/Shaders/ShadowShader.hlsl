#include"common.hlsl"

struct Light
{
	int4 type; // x = { 0 = Directional / 1 = Point / 2 = Spot }

	float4 position;
	float4 direction;
	float4 lightColor;
    
	// x = Intensity, y = Range, z = InnerCone, w = OuterCone
	float4 lightProp;
    
	matrix lightVPMatrix;
	matrix invLightVPMatrix;
};

cbuffer LightBuffer : register(b3)
{
	Light light[8];
};

struct VSInput
{
	uint id : SV_VertexID;
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

float4 VSMain(VSInput i) : SV_Position
{
    float4 wPos = mul(float4(i.pos, 1), modelMatrix);
    wPos = mul(wPos, light[0].lightVPMatrix);
    
	return wPos;
}

float4 PSMain() : SV_Target
{
	return float4(0.25, 0.5, 0.75, 1);

}