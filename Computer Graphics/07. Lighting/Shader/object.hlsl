#include "common.hlsl"

struct VERTEX_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

PIXEL_INPUT VERTEX_MAIN(VERTEX_INPUT input)
{
    PIXEL_INPUT output;
    output.position = mul(float4(input.position, 1.0f), g_worldMatrix);
    output.positionW = output.position.xyz;
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.normal = mul(input.normal, (float3x3)g_worldMatrix);
    output.uv = input.uv;
    
    return output;
}

float4 PIXEL_MAIN(PIXEL_INPUT input) : SV_TARGET
{
    float4 diffuse = g_texture[0].Sample(g_sampler, input.uv);
    return Lighting(input.positionW, input.normal, g_cameraPosition, diffuse, g_material[0]);
}