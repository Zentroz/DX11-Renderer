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
Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D materialTex : register(t2);
SamplerState samp : register(s0);
SamplerState samp1 : register(s1);
SamplerState samp2 : register(s2);


float4 NormalColorOutput(float4 normal)
{
    if (length(normal) == 0)
    {
        return float4(0, 0, 0, 1);
    }
    
    return (normal + 1) * 0.5;
}

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
    float2 ScreenSize = float2(1536, 793);
    float4 albedo = albedoTex.Sample(samp, pos.xy / ScreenSize);
    float4 normal = normalTex.Sample(samp1, pos.xy / ScreenSize);

    return albedo;
    //return NormalOutput(normal);
}