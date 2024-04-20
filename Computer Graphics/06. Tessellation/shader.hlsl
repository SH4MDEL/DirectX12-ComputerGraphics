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


struct TERRAIN_VERTEX_INPUT
{
    float3 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
    uint density : DENSITY;
};

struct TERRAIN_HULL_INPUT
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
    uint density : DENSITY;
};

struct TERRAIN_DOMAIN_INPUT
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct TERRAIN_PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

TERRAIN_HULL_INPUT TERRAIN_VERTEX(TERRAIN_VERTEX_INPUT input)
{
    TERRAIN_HULL_INPUT output;
    output.position = float4(input.position, 1.0f);
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;
    output.density = input.density;
    
    return output;
}

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

float GetMaxTessFactor(uint dencity)
{
    if (dencity == 0) return 1.f;
    if (dencity >= 1 && dencity <= 2) return 3.f;
    if (dencity >= 3 && dencity <= 5) return 10.f;
    return 64.f;

}

float CalcTessFactor(float4 center, uint maxDencity)
{
    float maxTessFactor = GetMaxTessFactor(maxDencity);
    float3 d = distance(g_cameraPosition, center.xyz); 
    const float d0 = 20.f;
    const float d1 = 100.f;
    return clamp(20.f * saturate((d1 - d) / (d1 - d0)), min(maxTessFactor, 3.f), min(maxTessFactor, 64.f));
}

PatchTess CONSTANT_TERRAIN_HULL(InputPatch<TERRAIN_HULL_INPUT, 25> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess output;

    uint maxDencity = 0;
    for (int i = 0; i < 24; ++i) maxDencity = max(maxDencity, patch[i].density);
    
    output.EdgeTess[0] = CalcTessFactor(0.5f * mul((patch[0].position + patch[4].position), g_worldMatrix), maxDencity);
    output.EdgeTess[1] = CalcTessFactor(0.5f * mul((patch[0].position + patch[20].position), g_worldMatrix), maxDencity);
    output.EdgeTess[2] = CalcTessFactor(0.5f * mul((patch[4].position + patch[24].position), g_worldMatrix), maxDencity);
    output.EdgeTess[3] = CalcTessFactor(0.5f * mul((patch[20].position + patch[24].position), g_worldMatrix), maxDencity);
    
    output.InsideTess[0] = CalcTessFactor(0.25f * mul((patch[0].position + patch[4].position + 
        patch[20].position + patch[24].position), g_worldMatrix), maxDencity);
    output.InsideTess[1] = output.InsideTess[0];
    
    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("CONSTANT_TERRAIN_HULL")]
[maxtessfactor(64.0f)]
TERRAIN_DOMAIN_INPUT TERRAIN_HULL(InputPatch<TERRAIN_HULL_INPUT, 25> p,
    uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    TERRAIN_DOMAIN_INPUT output;
    output.position = p[i].position;
    output.uv0 = p[i].uv0;
    output.uv1 = p[i].uv1;
    return output;
}

void BernsteinBasis(float t, out float basis[5])
{
    float invT = 1.f - t;
    basis[0] = invT * invT * invT * invT;
    basis[1] = 4.f * t * invT * invT * invT;
    basis[2] = 6.f * t * t * invT * invT;
    basis[3] = 4.f * t * t * t * invT;
    basis[4] = t * t * t * t;
}

float3 LineBezierSum(OutputPatch<TERRAIN_DOMAIN_INPUT, 25> patch, uint start, float basis[5])
{
    return basis[0] * patch[start].position +
    basis[1] * patch[start + 1].position +
    basis[2] * patch[start + 2].position +
    basis[3] * patch[start + 3].position +
    basis[4] * patch[start + 4].position;
}

float3 CubicBezierSum(OutputPatch<TERRAIN_DOMAIN_INPUT, 25> patch, float basisU[5], float basisV[5])
{
    float3 sum = float3(0.f, 0.f, 0.f);
    sum += basisV[0] * LineBezierSum(patch, 0, basisU);
    sum += basisV[1] * LineBezierSum(patch, 5, basisU);
    sum += basisV[2] * LineBezierSum(patch, 10, basisU);
    sum += basisV[3] * LineBezierSum(patch, 15, basisU);
    sum += basisV[4] * LineBezierSum(patch, 20, basisU);
    return sum;
}

[domain("quad")]
TERRAIN_PIXEL_INPUT TERRAIN_DOMAIN(PatchTess patchTess, 
    float2 domainLocation : SV_DomainLocation, 
    const OutputPatch<TERRAIN_DOMAIN_INPUT, 25> patch)
{
    TERRAIN_PIXEL_INPUT output;
    
    float basisU[5], basisV[5];
    BernsteinBasis(domainLocation.x, basisU);
    BernsteinBasis(domainLocation.y, basisV);
    
    output.position = float4(CubicBezierSum(patch, basisU, basisV), 1.f);
    output.position = mul(output.position, g_worldMatrix);
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    
    output.uv0 = lerp(
    lerp(patch[0].uv0, patch[4].uv0, domainLocation.x),
    lerp(patch[20].uv0, patch[24].uv0, domainLocation.x),
    domainLocation.y);
    output.uv1 = lerp(
    lerp(patch[0].uv1, patch[4].uv1, domainLocation.x),
    lerp(patch[20].uv1, patch[24].uv1, domainLocation.x),
    domainLocation.y);
    
    return output;
}

float4 TERRAIN_PIXEL(TERRAIN_PIXEL_INPUT input) : SV_TARGET
{
    return lerp(g_texture[0].Sample(g_sampler, input.uv0), 
    g_texture[1].Sample(g_sampler, input.uv1), 0.5f);
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