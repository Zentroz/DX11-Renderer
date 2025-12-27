cbuffer StaticData : register(b0)
{
    float4 mainLightDirection;
    float4 mainLightColor;
};
cbuffer FrameData : register(b1)
{
    matrix vpMatrix;
    float4 cameraPosition;
};
cbuffer ObjectData : register(b2)
{
    matrix modelMatrix;
};
cbuffer MaterialData : register(b2)
{
    float4 diffuseColor;
    float roughness;
    float metallic;
    float2 padding;
};
// Phone lighting material parameters
// cbuffer MaterialData : register(b2) {
//     float4 diffuseColor;
//     float4 ambientColor;
//     float4 specularColor;
//     float shininess;
// };

float3 GammaCorrection(float3 color, float gamma)
{
    float3 g3 = 1 / gamma;
    return pow(color, gamma);
}
float4 GammaCorrection(float4 color, float gamma)
{
    float4 g3 = 1 / gamma;
    return pow(color, gamma);
}