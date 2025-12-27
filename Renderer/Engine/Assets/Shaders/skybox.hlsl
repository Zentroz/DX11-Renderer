cbuffer SkyboxData : register(b0)
{
    matrix viewMatrix;
    matrix projMatrix;
};

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 texDir : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    // Remove translation from the view matrix
    float4x4 viewNoTranslation = viewMatrix;
    viewNoTranslation._41 = 0.0f;
    viewNoTranslation._42 = 0.0f;
    viewNoTranslation._43 = 0.0f;

    // Transform vertex
    float4 worldPos = float4(input.position, 1.0f);
    float4 viewPos = mul(worldPos, viewNoTranslation);
    float4 clipPos = mul(viewPos, projMatrix);

    output.position = clipPos;

    // Use the original position as direction
    output.texDir = input.position;

    return output;
}

Texture2D skyboxTexture : register(t0);
SamplerState skyboxSampler : register(s0);

float4 PSMain(VSOutput input) : SV_Target
{
    return skyboxTexture.Sample(skyboxSampler, input.texDir);
}