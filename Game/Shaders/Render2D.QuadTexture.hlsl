#include "Common.hlsli"

struct DrawConstants
{
    float4x4 Projection;
};

ConstantBuffer<DrawConstants> PushConstant : register(b0, space0);
Texture2D<float4> Texture : register(t1);
sampler Sampler : register(s2);

VS2DOutput VS_Main(VS2DInput input)
{
    VS2DOutput output;
    output.Position = mul(PushConstant.Projection, float4(input.Position, 0.0f, 1.0f));
    output.TexCoord = input.TexCoord;
    output.Color = input.Color;
    return output;
}

float4 PS_Main(VS2DOutput input) : SV_Target
{
    return Texture.Sample(Sampler, input.TexCoord);
}
