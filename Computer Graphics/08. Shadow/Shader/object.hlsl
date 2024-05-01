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
    nointerpolation uint textureIndex : TEXINDEX;
    nointerpolation uint materialIndex : MATINDEX;
};

PIXEL_INPUT VERTEX_MAIN(VERTEX_INPUT input, uint instanceID : SV_InstanceID)
{
    PIXEL_INPUT output;
    InstanceData instData = g_instanceData[instanceID];
    output.position = mul(float4(input.position, 1.0f), instData.worldMatrix);
    output.positionW = output.position.xyz;
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.normal = mul(input.normal, (float3x3)instData.worldMatrix);
    output.uv = input.uv;
    output.textureIndex = instData.textureIndex;
    output.materialIndex = instData.materialIndex;
    
    return output;
}

float4 PIXEL_MAIN(PIXEL_INPUT input) : SV_TARGET
{
    //return g_shadowMap.Sample(g_sampler, input.uv);
    float4 diffuse = g_texture[input.textureIndex].Sample(g_sampler, input.uv);
    return Lighting(input.positionW, input.normal, g_cameraPosition, diffuse, g_material[input.materialIndex]);
}

PIXEL_INPUT SHADOW_VERTEX_MAIN(VERTEX_INPUT input, uint instanceID : SV_InstanceID)
{
    PIXEL_INPUT output;
    InstanceData instData = g_instanceData[instanceID];
    output.position = mul(float4(input.position, 1.0f), instData.worldMatrix);
    output.position = mul(output.position, g_lightViewMatrix);
    output.position = mul(output.position, g_lightProjectionMatrix);
    return output;
}