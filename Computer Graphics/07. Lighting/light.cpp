#include "light.h"

Light::Light(UINT type, XMFLOAT3 strength, XMFLOAT3 direction) :
	m_type{type}, m_strength{strength}, m_direction{direction}, m_position{ 0.f, 0.f, 0.f },
	m_fallOffStart{ 0.f }, m_fallOffEnd{ 0.f }, m_spotPower{ 0.f }
{
	m_direction = Utiles::Vector3::Normalize(m_direction);
}

Light::Light(UINT type, XMFLOAT3 strength, XMFLOAT3 position, 
	FLOAT fallOffStart, FLOAT fallOffEnd) :
	m_type{ type }, m_strength{ strength }, m_direction{ 0.f, 0.f, 0.f }, m_position{ position },
	m_fallOffStart{ fallOffStart }, m_fallOffEnd{ fallOffEnd }, m_spotPower{ 0.f }
{
}

Light::Light(UINT type, XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position, 
	FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower) :
	m_type{ type }, m_strength{ strength }, m_direction{ direction }, m_position{ position },
	m_fallOffStart{ fallOffStart }, m_fallOffEnd{ fallOffEnd }, m_spotPower{ spotPower }
{
	m_direction = Utiles::Vector3::Normalize(m_direction);
}

void Light::UpdateShaderVariable(LightData& buffer)
{
	buffer.type = m_type;
	buffer.strength = m_strength;
	buffer.direction = m_direction;
	buffer.position = m_position;
	buffer.fallOffStart = m_fallOffStart;
	buffer.fallOffEnd = m_fallOffEnd;
	buffer.spotPower = m_spotPower;
}

void Light::SetStrength(XMFLOAT3 strength)
{
	m_strength = strength;
}

DirectionalLight::DirectionalLight() :
	Light(Settings::Light::Directional, { 1.f, 1.f, 1.f }, { 0.f, -1.f, 0.f })
{
}

DirectionalLight::DirectionalLight(XMFLOAT3 strength, XMFLOAT3 direction) :
	Light(Settings::Light::Directional, strength, direction)
{
}

void DirectionalLight::SetDirection(XMFLOAT3 direction)
{
	m_direction = Utiles::Vector3::Normalize(direction);
}

PointLight::PointLight() : 
	Light(Settings::Light::Point, {1.f, 1.f, 1.f}, { 0.f, 0.f, 0.f }, { 0.1f }, { 10.f })
{
}

PointLight::PointLight(XMFLOAT3 strength, XMFLOAT3 position, FLOAT fallOffStart, FLOAT fallOffEnd) :
	Light(Settings::Light::Point, strength, position, fallOffStart, fallOffEnd)
{
}

void PointLight::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void PointLight::SetFallOffStart(FLOAT fallOffStart)
{
	m_fallOffStart = fallOffStart;
}

void PointLight::SetFallOffEnd(FLOAT fallOffEnd)
{
	m_fallOffEnd = fallOffEnd;
}

SpotLight::SpotLight() :
	Light(Settings::Light::Spot, { 1.f, 1.f, 1.f }, {0.f, -1.f, 0.f}, 
		{ 0.f, 0.f, 0.f }, { 0.1f }, { 10.f }, {10.f})
{
}

SpotLight::SpotLight(XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position, 
	FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower) :
	Light(Settings::Light::Spot, strength, direction, position, 
		fallOffStart, fallOffEnd, spotPower)
{
}

void SpotLight::SetDirection(XMFLOAT3 direction)
{
	m_direction = Utiles::Vector3::Normalize(direction);
}

void SpotLight::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void SpotLight::SetFallOffStart(FLOAT fallOffStart)
{
	m_fallOffStart = fallOffStart;
}

void SpotLight::SetFallOffEnd(FLOAT fallOffEnd)
{
	m_fallOffEnd = fallOffEnd;
}

void SpotLight::SetSpotPower(FLOAT spotPower)
{
	m_spotPower = spotPower;
}

LightSystem::LightSystem(const ComPtr<ID3D12Device>& device)
{
	m_constantBuffer = make_unique<UploadBuffer<LightsData>>(device, (UINT)RootParameter::Light);
}

void LightSystem::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	LightsData buffer;
	int i = 0;
	for (const auto& light : m_lights) {
		light->UpdateShaderVariable(buffer.lights[i++]); }
	if (i != Settings::Light::MaxLight) buffer.lights[i].type = Settings::Light::Last;

	m_constantBuffer->Copy(buffer);
	m_constantBuffer->UpdateRootConstantBuffer(commandList);
}

void LightSystem::SetLight(const shared_ptr<Light>& light)
{
	if (m_lights.size() == Settings::Light::MaxLight) throw out_of_range("Limit Max Light");
	m_lights.push_back(light);
}