#include "Common.hlsli"

struct DrawConstants
{
    float3 Camera;
    float AOIntensity;
};

#define PI 3.14159265359

ConstantBuffer<DrawConstants> PushConstant : register(b0, space0);
Texture2D PositionBuffer : register(t0);
Texture2D NormalBuffer : register(t1);
Texture2D ColorBuffer : register(t2);
Texture2D ShadowMap : register(t3);
Texture2D IrradianceEnv : register(t4);
SamplerState Sampler : register(s0);

struct DirectionalLight
{
    float3 Direction;
    float3 Color;
};

FXOutput VS_Main(uint id : SV_VertexID)
{
    return FXVSPassthrough2(id);
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float2 sphericalCoords(float3 direction)
{
    float3 normalized = normalize(direction);
    float2 uv;
    uv.x = atan2(normalized.z, normalized.x) / (2.0 * 3.14159265) + 0.5;
    uv.y = acos(normalized.y) / 3.14159265;
    return float2(uv.x, 1 - uv.y);
}

float4 PS_Main(FXOutput input) : SV_Target
{
    GBuffer buffer = LoadGBuffer(input.TexCoord, PositionBuffer, NormalBuffer, ColorBuffer);
	//return float4(buffer.Normal * 0.5 + 0.5,1);


    DirectionalLight lights[1];

    lights[0].Direction = float3(-2, -1, -3.2f);
    lights[0].Color = float3(10, 9, 9);

    if (buffer.Color.a == 0)
        discard;

    float3 N = buffer.Normal;
    float3 V = normalize(PushConstant.Camera - buffer.Position);

    float shadow = ShadowMap.SampleLevel(Sampler, input.TexCoord, 0).r;

    float3 Lo = float3(0, 0, 0);

    for (int i = 0; i < 1; ++i)
    {
        DirectionalLight light = lights[i];

        float3 L = normalize(-light.Direction);
        float3 H = normalize(V + L);

        float3 F0 = float3(0.04, 0.04, 0.04);
        F0 = lerp(F0, buffer.Color, buffer.Metallic);
        float3 F = fresnelSchlick(max(dot(H, V), 0), F0);

        float NDF = DistributionGGX(N, H, buffer.Roughness);
        float G = GeometrySmith(N, V, L, buffer.Roughness);

        float3 numerator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0) * max(dot(N, L), 0) + 0.001f;
        float3 specular = numerator / denominator;

        float3 kS = F;
        float3 kD = float3(1, 1, 1) - kS;

        kD *= 1.0 - buffer.Metallic;

        float NdotL = max(dot(N, L), 0);
        Lo += shadow * (kD * buffer.Color / PI + specular) * light.Color * NdotL;
    }


    float3 color = Lo;
    {
        float3 F0 = float3(0.04f, 0.04f, 0.04f);
        float3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, buffer.Roughness);
        float3 kD = 1.0 - kS;
        float3 irradiance = IrradianceEnv.SampleLevel(Sampler, sphericalCoords(N), 0).rgb;
        float3 diffuse = irradiance * buffer.Color;
        float3 ambient = (kD * diffuse) * 1;

        color += ambient;
    }

    color = (color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14);
    //color = color / (color + 2.1);


    return float4(color, 1);
}
