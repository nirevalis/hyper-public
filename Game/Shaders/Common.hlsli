
struct VS2DInput
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
    uint Metadata : MY_SEMANTIC;
};

struct VS2DOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

struct VS2DOutputMetadata
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
    nointerpolation uint Metadata : MY_SEMANTIC;
};

struct FXInput
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct FXOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VS3DInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct VS3DOutput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct GBuffer
{
    float3 Position;
    float3 Normal;
    float4 Color;
    float Metallic;
    float Roughness;
};

struct GBufferOutput
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Color : SV_Target2;
};

struct Material
{
    float4 Color;
    float4 Normal;
    float Roughness;
    float Metallic;
};

GBuffer LoadGBuffer(float2 uv, Texture2D position, Texture2D normal, Texture2D color)
{
    GBuffer buffer;

    uint width, height;
    position.GetDimensions(width, height);

    int2 pixelCoord = int2(uv * float2(width, height));

    buffer.Position = position.Load(int3(pixelCoord, 0)).xyz;
    buffer.Normal = normal.Load(int3(pixelCoord, 0)).xyz;
    buffer.Color = color.Load(int3(pixelCoord, 0));

    buffer.Roughness = position.Load(int3(pixelCoord, 0)).w;
    buffer.Metallic = normal.Load(int3(pixelCoord, 0)).w;

    buffer.Normal = normalize(buffer.Normal);

    return buffer;
}

float SampleShadow(float2 uv, Texture2D shadowMap, SamplerState smp)
{
    return shadowMap.Sample(smp, uv).x;
}

GBuffer LoadGBufferRT(uint2 uv, Texture2D position, Texture2D normal, Texture2D color)
{
    GBuffer buffer;

    buffer.Position = position.Load(uint3(uv, 0));
    buffer.Normal = normalize(normal.Load(uint3(uv, 0)).xyz);
    buffer.Color = color.Load(uint3(uv, 0));

    return buffer;
}


FXOutput FXVSPassthrough(FXInput input)
{
    FXOutput output;
    output.Position = float4(input.Position, 0.0f, 1.0f);
    output.TexCoord = input.TexCoord;

    return output;
}

FXOutput FXVSPassthrough2(uint id)
{
    FXOutput output;
    output.TexCoord = float2((id << 1) & 2, id & 2);
    output.Position = float4(output.TexCoord.x * 2.0f - 1.0f, -output.TexCoord.y * 2.0f + 1.0f, 0.0f, 1.0f);

    return output;
}
