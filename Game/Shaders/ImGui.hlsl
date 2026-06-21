#include "Common.hlsli"

struct DrawConstants
{
    float2 InvDisplaySize;
};

ConstantBuffer<DrawConstants> PushConstant : register(b0, space0);
SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);

VS2DOutput VS_Main(VS2DInput input)
{
    VS2DOutput output;
    output.Position.xy = input.Position.xy * PushConstant.InvDisplaySize * float2(2.0, -2.0) + float2(-1.0, 1.0);
    output.Position.zw = float2(0, 1);
    output.TexCoord = input.TexCoord;
    output.Color = input.Color;
    return output;
}

float4 PS_Main(VS2DOutput input) : SV_Target
{
    float4 color = input.Color * Texture.Sample(Sampler, input.TexCoord);
    return input.Color;
}
 