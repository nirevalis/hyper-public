#include "Common.hlsli"

struct HitInfo
{
    bool Hit;
};

struct Attributes
{
    float2 Barycentric;
};

struct RayTraceConstants
{
    float Radius;
    float Power;
    float Bias;
    float _Padding0;
};

ConstantBuffer<RayTraceConstants> PushConstant : register(b0);

RWTexture2D<float4> RTOutput : register(u0);
RaytracingAccelerationStructure SceneBVH : register(t0);
Texture2D PositionBuffer : register(t1);
Texture2D NormalBuffer : register(t2);
Texture2D ColorBuffer : register(t3);

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16) | (bits >> 16);
    bits = ((bits & 0x55555555u) << 1) | ((bits & 0xAAAAAAAAu) >> 1);
    bits = ((bits & 0x33333333u) << 2) | ((bits & 0xCCCCCCCCu) >> 2);
    bits = ((bits & 0x0F0F0Fu) << 4) | ((bits & 0xF0F0F0F0u) >> 4);
    bits = ((bits & 0x00FF00FFu) << 8) | ((bits & 0xFF00FF00u) >> 8);
    return float(bits) * 2.3283064365386963e-10;
}

float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float Hash12(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031f);
    p3 += dot(p3, p3.yzx + 33.33f);
    return frac((p3.x + p3.y) * p3.z);
}

float3 CosineSampleHemisphere(float2 Xi)
{
    float r = sqrt(Xi.x);
    float theta = 6.28318530718f * Xi.y;

    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(max(0.0f, 1.0f - x * x - y * y));

    return float3(x, y, z);
}

void CreateONB(float3 n, out float3 tangent, out float3 bitangent)
{
    float3 up = abs(n.y) < 0.999f ? float3(0, 1, 0) : float3(1, 0, 0);
    tangent = normalize(cross(up, n));
    bitangent = cross(n, tangent);
}

[shader("raygeneration")]
void RG_Main()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 dims = DispatchRaysDimensions().xy;

    GBuffer buffer = LoadGBufferRT(
        launchIndex,
        PositionBuffer,
        NormalBuffer,
        ColorBuffer
    );

    if (buffer.Color.a == 0.0f)
    {
        RTOutput[launchIndex] = float4(1, 1, 1, 1);
        return;
    }

    float3 N = normalize(buffer.Normal);
    float3 P = buffer.Position;

    const uint NUM_SAMPLES = 64;

    float3 tangent, bitangent;
    CreateONB(N, tangent, bitangent);

    float occlusion = 0.0f;

    float random = Hash12(float2(launchIndex));

    [loop]
    for (uint i = 0; i < NUM_SAMPLES; i++)
    {
        float2 Xi = Hammersley(i, NUM_SAMPLES);
        Xi.y = frac(Xi.y + random);

        float3 localDir = CosineSampleHemisphere(Xi);

        float3 dir = normalize(
            localDir.x * tangent +
            localDir.y * bitangent +
            localDir.z * N
        );

        RayDesc ray;
        ray.Origin = P + N * PushConstant.Bias;
        ray.Direction = dir;
        ray.TMin = PushConstant.Bias;
        ray.TMax = PushConstant.Radius;

        HitInfo payload;
        payload.Hit = false;

        TraceRay(
            SceneBVH,
            RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
            RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,
            0xFF,
            0,
            0,
            0,
            ray,
            payload
        );

        if (payload.Hit)
        {
            occlusion += 1.0f;
        }
    }

    float ao = 1.0f - occlusion / float(NUM_SAMPLES);
    ao = pow(saturate(ao), PushConstant.Power);

    RTOutput[launchIndex] = float4(ao, ao, ao, 1.0f);
}

[shader("closesthit")]
void CL_Main(inout HitInfo payload : SV_RayPayload, in Attributes attr)
{
    payload.Hit = true;
}

[shader("miss")]
void MI_Main(inout HitInfo payload : SV_RayPayload)
{
    payload.Hit = false;
}