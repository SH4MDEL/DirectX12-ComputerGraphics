#include "common.hlsl"

struct VERTEX_INPUT
{
    float3 position : POSITION;
    float2 size : SIZE;
};

struct GEOMETRY_INPUT
{
    float4 position : POSITION;
    float2 size : SIZE;
    uint textureIndex : TEXINDEX;
};

struct PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    nointerpolation uint textureIndex : TEXINDEX;
};

GEOMETRY_INPUT VERTEX_MAIN(VERTEX_INPUT input, uint instanceID : SV_InstanceID)
{
    GEOMETRY_INPUT output;
    InstanceData instData = g_instanceData[instanceID];
    output.position = mul(float4(input.position, 1.0f), instData.worldMatrix);
    output.size = input.size;
    output.textureIndex = instData.textureIndex;
    return output;
}

[maxvertexcount(4)]
void GEOMETRY_MAIN(point GEOMETRY_INPUT input[1],
    uint primID : SV_PrimitiveID, inout TriangleStream<PIXEL_INPUT> outStream)
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

    PIXEL_INPUT output;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.position = mul(vertices[i], g_viewMatrix);
        output.position = mul(output.position, g_projectionMatrix);
        output.normal = front;
        output.uv = uv[i];
        output.textureIndex = input[0].textureIndex;
        outStream.Append(output);
    }
}

float4 PIXEL_MAIN(PIXEL_INPUT input) : SV_TARGET
{
    return g_texture[input.textureIndex].Sample(g_sampler, input.uv);
}