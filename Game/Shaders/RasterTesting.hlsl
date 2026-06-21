#include "Common.hlsli"

struct DrawConstants
{
    row_major float3x4 Transform;     // 48 bytes
    float4 NormalMatrix[3];           // 48 bytes (3x float4 rows, padded from float3)
    float4x4 View;                    // 64 bytes
    uint Material;                    // 4 bytes
    uint _Padding0;                   // 4 bytes (align next to 16 bytes)
    uint2 _Padding1;                  // 8 bytes
};

ConstantBuffer<DrawConstants> PushConstant : register(b0, space0);

VS3DOutput VS_Main(VS3DInput input)
{
    VS3DOutput output;
    output.WorldPosition = mul(PushConstant.Transform, float4(input.Position, 1.0f)); // float3
    output.Position = mul(PushConstant.View, float4(output.WorldPosition, 1.0f)); // float4
    output.TexCoord = input.TexCoord;
    //output.Normal = input.Normal;
	float3x3 normalMatrix = float3x3(
        PushConstant.NormalMatrix[0].xyz,
        PushConstant.NormalMatrix[1].xyz,
        PushConstant.NormalMatrix[2].xyz
    );

    output.Normal = mul(normalMatrix, input.Normal);
    return output;
}
