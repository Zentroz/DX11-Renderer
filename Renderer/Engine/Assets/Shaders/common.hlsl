cbuffer StaticData : register(b0)
{
    float4 mainLightDirection;
    float4 mainLightColor;
};
cbuffer FrameData : register(b1)
{
    matrix vpMatrix;
    float4 cameraPosition;
	float4 timeAndScreen; // x = delta, y = total, z = screenWidth, w = screenHeight
};
cbuffer ObjectData : register(b2)
{
    matrix modelMatrix;
};

float3 GammaCorrection(float3 color, float gamma)
{
    float3 g3 = 1 / gamma;
    return pow(color, g3);
}
float4 GammaCorrection(float4 color, float gamma)
{
    float4 g3 = 1 / gamma;
    return pow(color, g3);
}