#pragma once
#include "stdafx.h"
#include "buffer.h"

struct LightData
{
    XMFLOAT3 strength;
    FLOAT fallOffStart;
    XMFLOAT3 direction;
    FLOAT fallOffEnd;
    XMFLOAT3 position;
    FLOAT spotPower;
    UINT type;
    XMFLOAT3 padding;
};

class Light abstract
{
public:
    Light(UINT type, XMFLOAT3 strength, XMFLOAT3 direction);
    Light(UINT type, XMFLOAT3 strength, XMFLOAT3 position, FLOAT fallOffStart, FLOAT fallOffEnd);
    Light(UINT type, XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position,
        FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower);

    void UpdateShaderVariable(LightData& buffer);

    void SetStrength(XMFLOAT3 direction);

protected:
    const UINT        m_type;
    XMFLOAT3    m_strength;
    XMFLOAT3    m_direction;
    XMFLOAT3    m_position;
    FLOAT       m_fallOffStart;
    FLOAT       m_fallOffEnd;
    FLOAT       m_spotPower;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight();
    DirectionalLight(XMFLOAT3 strength, XMFLOAT3 direction);

    void SetDirection(XMFLOAT3 direction);
};

class PointLight : public Light
{
public:
    PointLight();
    PointLight(XMFLOAT3 strength, XMFLOAT3 position, FLOAT fallOffStart, FLOAT fallOffEnd);

    void SetPosition(XMFLOAT3 position);
    void SetFallOffStart(FLOAT fallOffStart);
    void SetFallOffEnd(FLOAT fallOffEnd);
};

class SpotLight : public Light
{
public:
    SpotLight();
    SpotLight(XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position, 
        FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower);

    void SetDirection(XMFLOAT3 direction);
    void SetPosition(XMFLOAT3 position);
    void SetFallOffStart(FLOAT fallOffStart);
    void SetFallOffEnd(FLOAT fallOffEnd);
    void SetSpotPower(FLOAT spotPower);
};

struct LightsData : public BufferBase
{
    LightData lights[Settings::Light::MaxLight];
};

class LightSystem
{
public:
    LightSystem(const ComPtr<ID3D12Device>& device);
    ~LightSystem() = default;

    void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

    void SetLight(const shared_ptr<Light>& light);

private:
    vector<shared_ptr<Light>> m_lights;
    unique_ptr<UploadBuffer<LightsData>> m_constantBuffer;
};

