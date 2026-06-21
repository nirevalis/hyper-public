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

float screenPxRange(float2 texCoord) {
    float width, height;
    Texture.GetDimensions(width, height);
    float2 unitRange = float2(4.0f, 4.0f) / float2(width, height);
    float2 screenTexSize = 1.0 / (abs(ddx(texCoord)) + abs(ddy(texCoord)));
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float4 PS_Main(VS2DOutput input) : SV_Target
{
    float3 msd = Texture.Sample(Sampler, input.TexCoord).rgb;

    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(input.TexCoord) * (sd - 0.5);

    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    return float4(input.Color.rgb, input.Color.a * opacity);
}
