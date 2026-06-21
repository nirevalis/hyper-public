#include "Common.hlsli"

#define SURFACE_SOLID 0

struct HitInfo
{
    float4 ShadedColorAndHitT : SHADED_COLOR_AND_HIT_T;
};

struct Attributes
{
    float2 Barycentric;
};

struct InstanceBufferIndices
{
    uint vertexBufferIndex;
    uint indexBufferIndex;
};

struct Vertex3D
{
    float3 Position; // 12 bytes
    float2 TexCoord; //  8 bytes
    float3 Normal; // 12 bytes
};


struct RTFrameData
{
    float4x4 View;
    float4x4 Perspective;
    float4x4 ViewInv;
    float4x4 PerspectiveInv;
    float3 ViewPosition;
};

struct RTMaterial
{
    float4 Color;
    int ColorMapIndex;

    float4 Normal;
    int NormalMapIndex;
};


RWTexture2D<float4> RTOutput : register(u0);
RaytracingAccelerationStructure SceneBVH : register(t0);
ConstantBuffer<RTFrameData> FrameData : register(b0);
StructuredBuffer<InstanceBufferIndices> InstanceData : register(t1);
Texture2D<float4> Texture : register(t2);
StructuredBuffer<RTMaterial> MaterialData : register(t3);
sampler Sampler : register(s0);

ByteAddressBuffer BindlessBuffers[] : register(t0, space1);
Texture2D BindlessTextures[] : register(t0, space2);

[shader("raygeneration")]
void RG_Main()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    RayDesc ray;
    ray.Origin = mul(FrameData.ViewInv, float4(0, 0, 0, 1));
    float4 target = mul(FrameData.PerspectiveInv, float4(d.x, -d.y, 1, 1));
    ray.Direction = mul(FrameData.ViewInv, float4(target.xyz, 0));
    ray.TMin = 0;
    ray.TMax = 100000;

    HitInfo payload;
    payload.ShadedColorAndHitT = float4(1, 0, 0, 0);

    TraceRay(
        SceneBVH,
        0,
        0xFF,
        0,
        0,
        0,
        ray,
        payload);

    RTOutput[launchIndex.xy] = float4(payload.ShadedColorAndHitT.rgb, 1.f);
    //RTOutput[LaunchIndex.xy] = float4(ray.Origin.xyz, 1.f);
}

Vertex3D LoadVertex(ByteAddressBuffer buffer, uint index)
{
    Vertex3D v;
    uint baseOffset = index * sizeof(Vertex3D);
    v.Position = asfloat(buffer.Load3(baseOffset + 0));
    v.TexCoord = asfloat(buffer.Load2(baseOffset + 12));
    v.Normal = asfloat(buffer.Load3(baseOffset + 20));

    return v;
}

float2 interpolate(float2 vertices[3], float3 bary)
{
    return vertices[0] * bary[0] + vertices[1] * bary[1] + vertices[2] * bary[2];
}

float3 interpolate(float3 vertices[3], float3 bary)
{
    return vertices[0] * bary[0] + vertices[1] * bary[1] + vertices[2] * bary[2];
}

Vertex3D BarycentricVertex(ByteAddressBuffer vertexBuffer, uint3 triIndices, Attributes attrib)
{
    Vertex3D vertex;

    Vertex3D v0 = LoadVertex(vertexBuffer, triIndices.x);
    Vertex3D v1 = LoadVertex(vertexBuffer, triIndices.y);
    Vertex3D v2 = LoadVertex(vertexBuffer, triIndices.z);

    float3 barycentrics;
    barycentrics.yz = attrib.Barycentric;
    barycentrics.x = 1.0 - (barycentrics.y + barycentrics.z);

    float3 positions[3];
    positions[0] = v0.Position;
    positions[1] = v1.Position;
    positions[2] = v2.Position;

    float2 uvs[3];
    uvs[0] = v0.TexCoord;
    uvs[1] = v1.TexCoord;
    uvs[2] = v2.TexCoord;

    float3 normals[3];
    normals[0] = v0.Normal;
    normals[1] = v1.Normal;
    normals[2] = v2.Normal;

    vertex.Position = interpolate(positions, barycentrics);
    vertex.TexCoord = interpolate(uvs, barycentrics);
    vertex.Normal = interpolate(normals, barycentrics);

    return vertex;
}

[shader("closesthit")]
void CL_Main(inout HitInfo payload : SV_RayPayload,
    Attributes attrib : SV_IntersectionAttributes)
{
    uint instanceID = InstanceID();
    uint primitiveIndex = PrimitiveIndex();

    InstanceBufferIndices indices = InstanceData.Load(instanceID);
    RTMaterial material = MaterialData.Load(instanceID);

    ByteAddressBuffer vertexBuffer = BindlessBuffers[NonUniformResourceIndex(indices.vertexBufferIndex)];
    ByteAddressBuffer indexBuffer = BindlessBuffers[NonUniformResourceIndex(indices.indexBufferIndex)];

    uint3 triIndices = indexBuffer.Load3(primitiveIndex * sizeof(uint3));

    Vertex3D vertex = BarycentricVertex(vertexBuffer, triIndices, attrib);


    float3 lightDirection = normalize(float3(-1, 1, 3));
    float3 lightColor = float3(1.0f, 1, 1.0f);

    float3 ambientColor = float3(0.01f, 0.01f, 0.01f);

    float3 surfaceNormal = normalize(vertex.Normal);

    float3 viewDirection = normalize(FrameData.ViewPosition - vertex.Position);
    
    float diffuseIntensity = max(0.0f, dot(surfaceNormal, -lightDirection));

  
    float3 reflectedLightDirection = reflect(lightDirection, surfaceNormal);

    float shininess = 256;

    float specularIntensity = pow(max(0.0f, dot(reflectedLightDirection, viewDirection)), shininess);

    float3 specularColor = float3(1.0f, 1.0f, 1.0f);

    float3 totalLightContribution = ambientColor
                                  + (lightColor * diffuseIntensity)
                                  + (specularColor * specularIntensity);


    float3 objectBaseColor = material.Color.xyz;
    if (material.ColorMapIndex >= 0)
    {
        objectBaseColor = BindlessTextures[material.ColorMapIndex].SampleLevel(Sampler, vertex.TexCoord, 0);
    }

    float3 finalShadedColor = objectBaseColor * totalLightContribution;

    payload.ShadedColorAndHitT = float4(finalShadedColor, payload.ShadedColorAndHitT.w);
}


[shader("miss")]
void MI_Main(inout HitInfo payload : SV_RayPayload)
{
    payload.ShadedColorAndHitT = float4(0, 0, 0, -1.f);
}