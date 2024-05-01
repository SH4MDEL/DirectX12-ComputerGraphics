cbuffer GameObject : register(b0)
{
    matrix g_worldMatrix : packoffset(c0);
};

cbuffer Camera : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
    float3 g_cameraPosition : packoffset(c8);
};

cbuffer Shadow : register(b2)
{
    matrix g_lightViewMatrix : packoffset(c0);
    matrix g_lightProjectionMatrix : packoffset(c4);
}

#include "lighting.hlsl"

TextureCube g_textureCube : register(t0);
Texture2D g_shadowMap : register(t1);
Texture2D g_texture[4] : register(t2);

struct InstanceData
{
    float4x4 worldMatrix;
    uint textureIndex;
    uint materialIndex;
};
StructuredBuffer<InstanceData> g_instanceData : register(t0, space1);

SamplerState g_sampler : register(s0);
SamplerComparisonState g_shadowSampler : register(s1);

float CalcShadowFactor(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;
    
    float depth = shadowPosH.z;
    
    uint width, height, numMips;
    g_shadowMap.GetDimensions(0, width, height, numMips);
    
    // Texel size.
    float dx = 1.f / (float)width;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.f, -dx), float2(dx, -dx),
        float2(-dx, 0.f), float2(0.f, 0.f), float2(dx, 0.f),
        float2(-dx, +dx), float2(0.f, +dx), float2(dx, +dx)
    };
    
    float output = 0.f;
    
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        output += g_shadowMap.SampleCmpLevelZero(g_shadowSampler,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return output / 9.f;
}

float4 Lighting(float3 objectPosition, float3 normal,
    float3 toEye, float4 diffuse, MaterialData material)
{
    float3 output = float3(0.f, 0.f, 0.f);
    
    matrix ndc;
    ndc._11_12_13_14 = float4(+0.5f, +0.0f, 0.f, 0.f);
    ndc._21_22_23_24 = float4(+0.0f, -0.5f, 0.f, 0.f);
    ndc._31_32_33_34 = float4(+0.0f, +0.0f, 1.f, 0.f);
    ndc._41_42_43_44 = float4(+0.5f, +0.5f, 0.f, 1.f);
    
    float4 shadowPos = float4(objectPosition, 1.0);
    shadowPos = mul(shadowPos, g_lightViewMatrix);
    shadowPos = mul(shadowPos, g_lightProjectionMatrix);
    shadowPos = mul(shadowPos, ndc);
    float shadowFactor = CalcShadowFactor(shadowPos);

    for (int i = 0; i < g_lightNum.x; ++i)
    {
        output += ComputeDirectionalLight(g_directionalLights[i], normal, toEye, diffuse.rgb, material) * shadowFactor;
    }
    for (int j = 0; j < g_lightNum.y; ++j)
    {
        output += ComputePointLight(g_pointLights[j], objectPosition, normal, toEye, diffuse.rgb, material);
    }
    for (int k = 0; k < g_lightNum.z; ++k)
    {
        output += ComputeSpotLight(g_spotLights[k], objectPosition, normal, toEye, diffuse.rgb, material);
    }
    
    float3 ambient = material.ambient * diffuse.rgb;
    output += ambient;
    
    return float4(output, diffuse.a);
}