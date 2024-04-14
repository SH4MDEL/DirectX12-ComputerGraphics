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

TextureCube g_textureCube : register(t0);
Texture2D g_texture[4] : register(t1);

struct InstanceData
{
    float4x4 worldMatrix;
    uint textureIndex;
};
StructuredBuffer<InstanceData> g_instanceData : register(t0, space1);

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
    return g_texture[0].Sample(g_sampler, input.uv);
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
    return lerp(g_texture[0].Sample(g_sampler, input.uv0), g_texture[1].Sample(g_sampler, input.uv1), 0.5f);
}


struct BILLBOARD_VERTEX_INPUT
{
    float3 position : POSITION;
    float2 size : SIZE;
    
};

struct BILLBOARD_GEOMETRY_INPUT
{
    float4 position : POSITION;
    float2 size : SIZE;
    uint textureIndex : TEXINDEX;
};

struct BILLBOARD_PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    nointerpolation uint textureIndex : TEXINDEX;
};

BILLBOARD_GEOMETRY_INPUT BILLBOARD_VERTEX(BILLBOARD_VERTEX_INPUT input, uint instanceID : SV_InstanceID)
{
    BILLBOARD_GEOMETRY_INPUT output;
    InstanceData instData = g_instanceData[instanceID];
    output.position = mul(float4(input.position, 1.0f), instData.worldMatrix);
    output.size = input.size;
    output.textureIndex = instData.textureIndex;
    return output;
}

[maxvertexcount(4)]
void BILLBOARD_GEOMETRY(point BILLBOARD_GEOMETRY_INPUT input[1],
    uint primID : SV_PrimitiveID, inout TriangleStream<BILLBOARD_PIXEL_INPUT> outStream)
{
    float3 up = float3(0.f, 1.f, 0.f);
    float3 front = normalize(g_cameraPosition - input[0].position.xyz);
    float3 right = cross(up, front);
    
    float halfWidth = input[0].size.x * 0.5f;
    float halfHeight = input[0].size.y * 0.5f;

    float4 vertices[4];
    vertices[0] = float4(input[0].position.xyz + halfWidth * right - halfHeight * up, 1.0f);
    vertices[1] = float4(input[0].position.xyz + halfWidth * right + halfHeight * up, 1.0f);
    vertices[2] = float4(input[0].position.xyz - halfWidth * right - halfHeight * up, 1.0f);
    vertices[3] = float4(input[0].position.xyz - halfWidth * right + halfHeight * up, 1.0f);
    float2 uv[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };

    BILLBOARD_PIXEL_INPUT output;
    [unroll]
    for (int i = 0; i < 4; ++i) {
        output.position = mul(vertices[i], g_viewMatrix);
        output.position = mul(output.position, g_projectionMatrix);
        output.normal = front;
        output.uv = uv[i];
        output.textureIndex = input[0].textureIndex;
        outStream.Append(output);
    }
}

float4 BILLBOARD_PIXEL(BILLBOARD_PIXEL_INPUT input) : SV_TARGET
{
    return g_texture[input.textureIndex].Sample(g_sampler, input.uv);
}