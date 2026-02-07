struct Light
{
	int4 type; // x = { 0 = Directional / 1 = Point / 2 = Spot }

	float4 position;
	float4 direction;
	float4 lightColor;
    
	// x = Intensity, y = Range, z = InnerCone, w = OuterCone
	float4 lightProp;
};

cbuffer LightBuffer : register(b3)
{
	Light lights[8];
	int4 lightCount;
}

#define PI 3.14159265359

float3 GetLightDirection(uint index, float3 position)
{
	Light light = lights[index];
    
	if (light.type.x == 0)
	{
        // Directional
		return -light.direction;
	}
	else if (light.type.x == 1)
	{
        // Point
		return normalize(light.position.xyz - position);
	}
	else
	{
        // Spot
		return normalize(light.position.xyz - position);
	}
}
float3 GetLightDirection(Light light, float3 position)
{
	if (light.type.x == 0)
	{
        // Directional
		return light.direction;
	}
	else if (light.type.x == 1)
	{
        // Point
		return normalize(light.position.xyz - position);
	}
	else
	{
        // Spot
		return normalize(light.position.xyz - position);
	}
}

float CalculateLightIntensity(Light light, float3 position)
{
	float intensity = 0;
    
	float3 lightDir = position - light.position.xyz;
	float dist = length(lightDir);
    
	if (light.type.x == 0)
	{
        // Directional
		return light.lightProp.x;
	}
	else if (light.type.x == 1)
	{
        // Point
		float3 lightDir = position - light.position.xyz;
		float dist = length(lightDir);
    
		float range = light.lightProp.y;
		float distSq = dist * dist;

		float attenuation = 1.0 / max(distSq, 0.001);
        
		float fade = saturate(1.0 - dist / range);
		fade *= fade;

		intensity = light.lightProp.x * attenuation * fade;
	}
	else
	{
        // Spot
		float angle = dot(light.direction.xyz, normalize(lightDir));
		intensity = clamp((angle - light.lightProp.w) / (light.lightProp.z - light.lightProp.w), 0.0f, 1.0f);
	}
        
	return intensity;
}

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
    float lightIntensity,
    float3 lightColor
)
{
	float3 radiance = lightColor * lightIntensity;
    
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

    return Lo;
}