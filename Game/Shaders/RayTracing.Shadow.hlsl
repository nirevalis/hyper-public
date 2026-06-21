#include "Common.hlsli"

struct HitInfo
{
    bool Missed;
};

struct RayTraceConstants
{
    float3 SunDirection;
};

struct Attributes
{
    float2 Barycentric;
};

ConstantBuffer<RayTraceConstants> PushConstant : register(b0);

RWTexture2D<float> RTOutput : register(u0);
RaytracingAccelerationStructure SceneBVH : register(t0);
Texture2D PositionBuffer : register(t1);
Texture2D NormalBuffer : register(t2);
Texture2D ColorBuffer : register(t3);

[shader("raygeneration")]
void RG_Main()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 dims = DispatchRaysDimensions().xy;

    if (launchIndex.x >= dims.x || launchIndex.y >= dims.y)
        return;

    GBuffer buffer = LoadGBufferRT(launchIndex, PositionBuffer, NormalBuffer, ColorBuffer);

    float3 sunDir = normalize(-PushConstant.SunDirection);

    float normalBias = 1e-3f;
    float3 offsetOrigin = buffer.Position.xyz + normalize(buffer.Normal) * normalBias;

    RayDesc ray;
    ray.Origin = offsetOrigin;
    ray.Direction = sunDir;
    ray.TMin = 0.001f;
    ray.TMax = 1000.0f;

    HitInfo payload;
    payload.Missed = false;

    TraceRay(
        SceneBVH,
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES,
        0xFF,
        0,
        0,
        0,
        ray,
        payload
    );

    float shadow = payload.Missed ? 1.0f : 0.0f;

    RTOutput[launchIndex] = shadow;
}

[shader("miss")]
void MI_Main(inout HitInfo payload : SV_RayPayload)
{
    payload.Missed = true;
}

[shader("closesthit")]
void CL_Main(inout HitInfo payload : SV_RayPayload, in Attributes attr)
{
    payload.Missed = false;
}
