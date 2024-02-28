cbuffer GameObject : register(b0)
{
    matrix g_worldMatrix : packoffset(c0);
};

cbuffer Camera : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
};

Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
TextureCube g_textureCube : register(t2);

SamplerState g_sampler : register(s0);

struct OBJECT_VERTEX_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct OBJECT_PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

OBJECT_PIXEL_INPUT OBJECT_VERTEX(OBJECT_VERTEX_INPUT input)
{
    OBJECT_PIXEL_INPUT output;
    output.position = mul(float4(input.position, 1.0f), g_worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.uv = input.uv;
    
    return output;
}

float4 OBJECT_PIXEL(OBJECT_PIXEL_INPUT input) : SV_TARGET
{
    return g_texture0.Sample(g_sampler, input.uv);
}


struct SKYBOX_VERTEX_INPUT
{
    float3 position : POSITION;
};

struct SKYBOX_PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float3 lookup : LOOKUP;
};

SKYBOX_PIXEL_INPUT SKYBOX_VERTEX(SKYBOX_VERTEX_INPUT input)
{
    SKYBOX_PIXEL_INPUT output;
    output.position = mul(float4(input.position, 1.0f), g_worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix).xyww;
    output.lookup = input.position;

    return output;
}

float4 SKYBOX_PIXEL(SKYBOX_PIXEL_INPUT input) : SV_TARGET
{
    return g_textureCube.Sample(g_sampler, input.lookup);
}


struct DETAIL_VERTEX_INPUT
{
    float3 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct DETAIL_PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

DETAIL_PIXEL_INPUT DETAIL_VERTEX(DETAIL_VERTEX_INPUT input)
{
    DETAIL_PIXEL_INPUT output;
    output.position = mul(float4(input.position, 1.0f), g_worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return output;
}

float4 DETAIL_PIXEL(DETAIL_PIXEL_INPUT input) : SV_TARGET
{
    return lerp(g_texture0.Sample(g_sampler, input.uv0), g_texture1.Sample(g_sampler, input.uv1), 0.5f);
}