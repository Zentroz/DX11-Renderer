struct Light
{
    float3 color;
    float intensity;
    float3 vec;
    float range;
};

cbuffer AdditionalLightData : register(b3)
{
    int lightCount;
    float3 adPadding;
    
    Light lights[32];
};

Light GetAdditionalLight(int index)
{
    return lights[index];
}

int GetAdditionalLightCount()
{
    return lightCount;
}

#define PI 3.14159265359

float3 CalculatePhongLighting(float3 normal, float3 fragPos, float3 camPos, float3 lightDir, float3 lightColor, float3 specularColor, float shininess)
{
    float ambientStrength = 0.1f;
    float3 ambient = ambientStrength * lightColor;

    float3 L = normalize(lightDir);
    float3 diff = max(ambient, dot(normal, L));

    float3 viewDir = normalize(camPos - fragPos);
    float3 reflectDir = reflect(-L, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float3 specular = shininess * spec * specularColor;

    return ambient + diff + specular;
}

// Distribution
float DGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / max(PI * denom * denom, 0.000001);
}

// GeometrySchlick
float GSGGX(float NdotX, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float numerator = NdotX;
    float denominator = NdotX * (1.0 - k) + k;

    return numerator / max(denominator, 0.000001);
}
// GeometrySmith
float GS(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GSGGX(NdotL, roughness);
    float ggx2 = GSGGX(NdotV, roughness);
	
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 CookTorranceBRDF(
    float3 albedo,
    float metallic,
    float roughness,
    float3 N,
    float3 V,
    float3 L,
    float3 lightColor
)
{
    float3 radiance = lightColor * 1;
    
    float3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    // Base reflectivity
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    
    float D = DGGX(N, H, roughness);
    float G = GS(N, V, L, roughness);
    float3 F = FresnelSchlick(HdotV, F0);

    float3 specular = D * G * F;
    specular /= 4.0 * NdotV * NdotL + 0.0001;
    
    float3 kD = 1 - F;
    kD *= 1 - metallic;
    
    float3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    float3 ambient = 0.3 * albedo;
    float3 color = ambient + Lo;
    
    //float3 p = 1.0 / 2.2;
    //color = color / (color + 1.0);
    //color = pow(color, p);

    return color;
}