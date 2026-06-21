#include "Common.hlsli"

struct DrawConstants
{
    float3 Camera;
    float Time;
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
    uv.x = atan2(normalized.z, normalized.x) / (2.0 * PI) + 0.5;
    uv.y = acos(normalized.y) / PI;
    return float2(uv.x, 1 - uv.y);
}

float4 PS_Main(FXOutput input) : SV_Target
{
    GBuffer buffer = LoadGBuffer(input.TexCoord, PositionBuffer, NormalBuffer, ColorBuffer);

    if (buffer.Color.a == 0)
        discard;

    float3 N = normalize(buffer.Normal);
    float3 V = normalize(PushConstant.Camera - buffer.Position); // viewDir

    DirectionalLight light;
    light.Direction = normalize(float3(-2, -1, -3.2f)); // matches Unity's _WorldSpaceLightPos0
    light.Color = float3(1, 0.95, 0.9); // white-ish light

    // 1. Diffuse Lighting
    float3 L = -light.Direction; // direction *to* light
    float NdotL = dot(N, L);
    float shadow = 1;

    float lightIntensity = smoothstep(0.0, 0.01, NdotL * shadow);
    float3 lightColor = light.Color * lightIntensity;

    // 2. Specular (Blinn)
    float3 H = normalize(V + L);
    float NdotH = max(dot(N, H), 0.0);

    float glossiness = 0.6; // _Glossiness equivalent
    float specularIntensity = pow(NdotH * lightIntensity, glossiness * glossiness);
    float specularIntensitySmooth = smoothstep(0.005, 0.01, specularIntensity);
    float3 specularColor = float3(1, 1, 1); // _SpecularColor
    float3 specular = specularColor * specularIntensitySmooth;

    // 3. Rim lighting
    float rimDot = 1.0 - dot(V, N);
    float rimThreshold = 1.0; // _RimThreshold
    float rimAmount = 0.75;   // _RimAmount
    float rimIntensity = rimDot * pow(NdotL, rimThreshold);
    rimIntensity = smoothstep(rimAmount - 0.01, rimAmount + 0.01, rimIntensity);
    float3 rimColor = float3(1.0, 0.9, 0.8); // _RimColor
    float3 rim = rimColor * rimIntensity;

    // 4. Final composition
    float3 ambientColor = float3(0.07, 0.07, 0.07); // _AmbientColor
    float3 baseColor = buffer.Color.rgb;           // sampled _MainTex * _Color

    float3 finalColor = (lightColor + ambientColor + specular + rim) * baseColor;

    return float4(finalColor, 1.0);
}
