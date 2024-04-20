#include "common.hlsl"

struct VERTEX_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PIXEL_INPUT VERTEX_MAIN(VERTEX_INPUT input)
{
    PIXEL_INPUT output;
    output.position = mul(float4(input.position, 1.0f), g_worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.uv = input.uv;
    
    return output;
}

float4 PIXEL_MAIN(PIXEL_INPUT input) : SV_TARGET
{
    return g_texture[0].Sample(g_sampler, input.uv);
}