#include "common.hlsl"

struct VERTEX_INPUT
{
    float3 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
    uint density : DENSITY;
};

struct HULL_INPUT
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
    uint density : DENSITY;
};

struct DOMAIN_INPUT
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normal : NORMAL;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

HULL_INPUT VERTEX_MAIN(VERTEX_INPUT input)
{
    HULL_INPUT output;
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
    if (dencity == 0)
        return 1.f;
    if (dencity >= 1 && dencity <= 2)
        return 3.f;
    if (dencity >= 3 && dencity <= 5)
        return 10.f;
    return 64.f;
}

float CalcTessFactor(float4 center, uint maxDencity)
{
    float maxTessFactor = GetMaxTessFactor(maxDencity);
    maxTessFactor = 64.f;
    float3 d = distance(g_cameraPosition, center.xyz);
    const float d0 = 20.f;
    const float d1 = 100.f;
    return clamp(20.f * saturate((d1 - d) / (d1 - d0)), min(maxTessFactor, 3.f), min(maxTessFactor, 64.f));
}

PatchTess CONSTANT_HULL(InputPatch<HULL_INPUT, 25> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess output;

    uint maxDencity = 0;
    for (int i = 0; i < 25; ++i)
        maxDencity = max(maxDencity, patch[i].density);
    
    output.EdgeTess[0] = CalcTessFactor(0.5f * mul((patch[0].position + patch[4].position), g_worldMatrix), maxDencity);
    output.EdgeTess[1] = CalcTessFactor(0.5f * mul((patch[0].position + patch[20].position), g_worldMatrix), maxDencity);
    output.EdgeTess[2] = CalcTessFactor(0.5f * mul((patch[4].position + patch[24].position), g_worldMatrix), maxDencity);
    output.EdgeTess[3] = CalcTessFactor(0.5f * mul((patch[20].position + patch[24].position), g_worldMatrix), maxDencity);
    
    float3 sum = float3(0.f, 0.f, 0.f);
    
    output.InsideTess[0] = CalcTessFactor(0.25f * mul((patch[0].position + patch[4].position +
        patch[20].position + patch[24].position), g_worldMatrix), maxDencity);
    output.InsideTess[1] = output.InsideTess[0];
    
    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("CONSTANT_HULL")]
[maxtessfactor(64.0f)]
DOMAIN_INPUT HULL_MAIN(InputPatch<HULL_INPUT, 25> p,
    uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DOMAIN_INPUT output;
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

float3 LineBezierSum(OutputPatch<DOMAIN_INPUT, 25> patch, uint start, float basis[5])
{
    return basis[0] * patch[start].position +
    basis[1] * patch[start + 1].position +
    basis[2] * patch[start + 2].position +
    basis[3] * patch[start + 3].position +
    basis[4] * patch[start + 4].position;
}

float3 CubicBezierSum(OutputPatch<DOMAIN_INPUT, 25> patch, float basisU[5], float basisV[5])
{
    float3 sum = float3(0.f, 0.f, 0.f);
    sum = basisV[0] * LineBezierSum(patch, 0, basisU);
    sum += basisV[1] * LineBezierSum(patch, 5, basisU);
    sum += basisV[2] * LineBezierSum(patch, 10, basisU);
    sum += basisV[3] * LineBezierSum(patch, 15, basisU);
    sum += basisV[4] * LineBezierSum(patch, 20, basisU);
    return sum;
}

void dBernsteinBasis(float t, out float basis[5])
{
    float invT = 1.f - t;
    basis[0] = -4.f * invT * invT * invT;
    basis[1] = 4.f * invT * invT * invT - 12.f * t * invT * invT;
    basis[2] = -12.f * t * t * invT + 12.f * invT * invT * t;
    basis[3] = -4.f * t * t * t + 12.f * t * t * invT;
    basis[4] = 4.f * t * t * t;
}

float3 dULineBezierSum(OutputPatch<DOMAIN_INPUT, 25> patch, 
    float basisV[5], float u)
{
    float3 curve[5];
    for (int i = 0; i < 5; ++i) {
        curve[i] = basisV[0] * patch[i].position +
                    basisV[1] * patch[i + 5].position +
                    basisV[2] * patch[i + 10].position +
                    basisV[3] * patch[i + 15].position +
                    basisV[4] * patch[i + 20].position;
    }

    float dbasisU[5];
    dBernsteinBasis(u, dbasisU);
    
    return curve[0] * dbasisU[0] +
        curve[1] * dbasisU[1] +
        curve[2] * dbasisU[2] +
        curve[3] * dbasisU[3] +
        curve[4] * dbasisU[4];
}

float3 dVLineBezierSum(OutputPatch<DOMAIN_INPUT, 25> patch,
    float basisU[5], float v) 
{
    float3 curve[5];
    for (int i = 0; i < 5; ++i) {
        curve[i] = basisU[0] * patch[i * 5].position +
                    basisU[1] * patch[i * 5 + 1].position +
                    basisU[2] * patch[i * 5 + 2].position +
                    basisU[3] * patch[i * 5 + 3].position +
                    basisU[4] * patch[i * 5 + 4].position;
    }

    float dbasisV[5];
    dBernsteinBasis(v, dbasisV);
    
    return curve[0] * dbasisV[0] +
        curve[1] * dbasisV[1] +
        curve[2] * dbasisV[2] +
        curve[3] * dbasisV[3] +
        curve[4] * dbasisV[4];
}

[domain("quad")]
PIXEL_INPUT DOMAIN_MAIN(PatchTess patchTess,
    float2 domainLocation : SV_DomainLocation,
    const OutputPatch<DOMAIN_INPUT, 25> patch)
{
    PIXEL_INPUT output;
    
    float basisU[5], basisV[5];
    BernsteinBasis(domainLocation.x, basisU);
    BernsteinBasis(domainLocation.y, basisV);
    
    output.position = float4(CubicBezierSum(patch, basisU, basisV), 1.f);
    output.position = mul(output.position, g_worldMatrix);
    output.positionW = output.position.xyz;
    output.position = mul(output.position, g_viewMatrix);
    output.position = mul(output.position, g_projectionMatrix);
    
    output.normal = normalize(cross(dULineBezierSum(patch, basisV, domainLocation.x),
                                    dVLineBezierSum(patch, basisU, domainLocation.y)));
    output.normal = mul(output.normal, (float3x3)g_worldMatrix);    
    
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

float4 PIXEL_MAIN(PIXEL_INPUT input) : SV_TARGET
{
    //return float4(input.normal, 1.f);
    float4 diffuse = lerp(g_texture[0].Sample(g_sampler, input.uv0),
        g_texture[1].Sample(g_sampler, input.uv1), 0.5f);
    return Lighting(input.positionW, input.normal, g_cameraPosition, diffuse, g_material[0]);
}