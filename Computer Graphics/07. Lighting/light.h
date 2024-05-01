#pragma once
#include "stdafx.h"
#include "buffer.h"

class Light
{
public:
    Light();
    Light(XMFLOAT3 strength);

    void SetStrength(XMFLOAT3 strength);

protected:
    XMFLOAT3    m_strength;
};

struct DirectionalLightData
{
    XMFLOAT3 strength;
    UINT padding0;
    XMFLOAT3 direction;
    UINT padding1;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight();
    DirectionalLight(XMFLOAT3 strength, XMFLOAT3 direction);

    void UpdateShaderVariable(DirectionalLightData& buffer);

    void SetDirection(XMFLOAT3 direction);

private:
    XMFLOAT3    m_direction;
};

struct PointLightData
{
    XMFLOAT3 strength;
    FLOAT fallOffStart;
    XMFLOAT3 position;
    FLOAT fallOffEnd;
};

class PointLight : public Light
{
public:
    PointLight();
    PointLight(XMFLOAT3 strength, XMFLOAT3 position, FLOAT fallOffStart, FLOAT fallOffEnd);

    void UpdateShaderVariable(PointLightData& buffer);

    void SetPosition(XMFLOAT3 position);
    void SetFallOffStart(FLOAT fallOffStart);
    void SetFallOffEnd(FLOAT fallOffEnd);

private:
    XMFLOAT3    m_position;
    FLOAT       m_fallOffStart;
    FLOAT       m_fallOffEnd;
};

struct SpotLightData
{
    XMFLOAT3 strength;
    FLOAT fallOffStart;
    XMFLOAT3 direction;
    FLOAT fallOffEnd;
    XMFLOAT3 position;
    FLOAT spotPower;
};

class SpotLight : public Light
{
public:
    SpotLight();
    SpotLight(XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position, 
        FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower);

    void UpdateShaderVariable(SpotLightData& buffer);

    void SetDirection(XMFLOAT3 direction);
    void SetPosition(XMFLOAT3 position);
    void SetFallOffStart(FLOAT fallOffStart);
    void SetFallOffEnd(FLOAT fallOffEnd);
    void SetSpotPower(FLOAT spotPower);

private:
    XMFLOAT3    m_direction;
    XMFLOAT3    m_position;
    FLOAT       m_fallOffStart;
    FLOAT       m_fallOffEnd;
    FLOAT       m_spotPower;
};

struct LightData : public BufferBase
{
    XMUINT4 lightNum;
    DirectionalLightData directionalLights[Settings::MaxDirectionalLight];
    PointLightData pointLights[Settings::MaxPointLight];
    SpotLightData spotLights[Settings::MaxSpotLight];
};

class LightSystem
{
public:
    LightSystem(const ComPtr<ID3D12Device>& device);
    ~LightSystem() = default;

    void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

    void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight);
    void SetPointLight(shared_ptr<PointLight> pointLight);
    void SetSpotLight(shared_ptr<SpotLight> spotLight);

private:
    XMUINT4 m_lightNum;
    vector<shared_ptr<DirectionalLight>> m_directionalLights;
    vector<shared_ptr<PointLight>> m_pointLights;
    vector<shared_ptr<SpotLight>> m_spotLights;

    unique_ptr<UploadBuffer<LightData>> m_constantBuffer;
};

