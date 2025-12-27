cbuffer SkyboxData : register(b0)
{
    matrix viewMatrix;
    matrix projMatrix;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 texDir : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    
    float4x4 viewNoTranslation = viewMatrix;
    viewNoTranslation._41 = 0.0f;
    viewNoTranslation._42 = 0.0f;
    viewNoTranslation._43 = 0.0f;

    float4 worldPos = float4(input.pos, 1.0f);
    float4 viewPos = mul(worldPos, viewNoTranslation);
    float4 clipPos = mul(viewPos, projMatrix);

    output.position = clipPos;
    output.texDir = input.pos;

    return output;
}

TextureCube skyboxTexture : register(t0);
SamplerState skyboxSampler : register(s0);

float4 PSMain(VSOutput input) : SV_Target
{
    return skyboxTexture.Sample(skyboxSampler, input.texDir);
}