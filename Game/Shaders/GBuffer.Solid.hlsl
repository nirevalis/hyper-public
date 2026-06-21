#include "Common.hlsli"

struct DrawConstants
{
    row_major float3x4 Transform; // 64 bytes
    row_major float3x3 NormalMatrix;
    float4x4 View; // 48 bytes
    uint Material; // 4 bytes
    uint3 _Padding; // 12 bytes to align to 128
};


ConstantBuffer<DrawConstants> PushConstant : register(b0, space0);
StructuredBuffer<Material> Materials : register(t1, space0);

GBufferOutput PS_Main(VS3DOutput input)
{
    GBufferOutput output;

    Material material = Materials.Load(PushConstant.Material);

    output.Position = float4(input.WorldPosition, material.Roughness);
    output.Normal = float4(input.Normal, material.Metallic);
    output.Color = material.Color;

    return output;
}
 