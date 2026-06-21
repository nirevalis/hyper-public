#include "Common.hlsli"

struct SkyConstants
{
    float4x4 View;
    float4x4 Projection;
};

ConstantBuffer<SkyConstants> PushConstant : register(b0);
Texture2D<float4> HDRI : register(t0);
Texture2D<float4> IRDHDRI : register(t1);
SamplerState Sampler : register(s0);
//TextureCube Radiance : register(t2);

VS3DOutput VS_Main(uint vertex : SV_VertexID)
{
    VS3DOutput output;

    float3 positions[36] =
    {
        // Front face
        float3(-1, -1, 1), float3(1, -1, 1), float3(1, 1, 1),
        float3(1, 1, 1), float3(-1, 1, 1), float3(-1, -1, 1),
        
        // Back face
        float3(-1, -1, -1), float3(-1, 1, -1), float3(1, 1, -1),
        float3(1, 1, -1), float3(1, -1, -1), float3(-1, -1, -1),
        
        // Left face
        float3(-1, -1, -1), float3(-1, -1, 1), float3(-1, 1, 1),
        float3(-1, 1, 1), float3(-1, 1, -1), float3(-1, -1, -1),
        
        // Right face
        float3(1, -1, -1), float3(1, 1, -1), float3(1, 1, 1),
        float3(1, 1, 1), float3(1, -1, 1), float3(1, -1, -1),
        
        // Top face
        float3(-1, 1, -1), float3(-1, 1, 1), float3(1, 1, 1),
        float3(1, 1, 1), float3(1, 1, -1), float3(-1, 1, -1),
        
        // Bottom face
        float3(-1, -1, -1), float3(1, -1, -1), float3(1, -1, 1),
        float3(1, -1, 1), float3(-1, -1, 1), float3(-1, -1, -1)
    };

    float3x3 rotation = (float3x3) PushConstant.View;

    float4x4 view = float4x4(
	    float4(rotation[0], 0.0),
	    float4(rotation[1], 0.0),
	    float4(rotation[2], 0.0),
	    float4(0.0, 0.0, 0.0, 1.0)
	);

    output.WorldPosition = positions[vertex];
    output.Position = mul(PushConstant.Projection, mul(view, float4(output.WorldPosition, 1)));
    output.Position.z = output.Position.w;

    return output;
}

float2 sphericalCoords(float3 direction)
{
    float3 normalized = normalize(direction);
    float2 uv;
    uv.x = atan2(normalized.z, normalized.x) / (2.0 * 3.14159265) + 0.5;
    uv.y = acos(normalized.y) / 3.14159265;
    return uv;
}

float4 PS_Main(VS3DOutput input) : SV_Target
{
    float2 uv = sphericalCoords(input.WorldPosition);
    float3 color = HDRI.SampleLevel(Sampler, uv, 0).rgb;
    color = (color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14);

    //color = clamp(color, float3(0, 0, 0), float3(1, 1, 1));

    return float4(color, 1);
}
