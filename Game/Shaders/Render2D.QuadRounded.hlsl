#include "Common.hlsli"

struct DrawConstants
{
    float4x4 Projection;
};

struct Metadata
{
    float Width;
    float Height;
    float Radius;
};

ConstantBuffer<DrawConstants> PushConstant : register(b0, space0);
StructuredBuffer<Metadata> Metadata : register(t0, space0);

VS2DOutputMetadata VS_Main(VS2DInput input)
{
    VS2DOutputMetadata output;
    output.Position = mul(PushConstant.Projection, float4(input.Position, 0.0f, 1.0f));
    output.TexCoord = input.TexCoord;
    output.Color = input.Color;
    output.Metadata = input.Metadata;
    return output;
}

float calc(float2 p, float2 b, float r)
{
    return length(max(abs(p) - b, 0.0)) - r;
}

float4 PS_Main(VS2DOutputMetadata input) : SV_Target
{
    struct Metadata m = Metadata.Load(input.Metadata);

    float2 size = float2(m.Width, m.Height);
    float2 pixel = input.TexCoord * size;
    float2 centre = 0.5 * size;
    float sa = smoothstep(0.0, 1, calc(centre - pixel, centre - m.Radius - 1, m.Radius));
    float4 c = lerp(float4(input.Color.rgb, input.Color.a), float4(input.Color.rgb, 0), sa);

    return c;
}
 