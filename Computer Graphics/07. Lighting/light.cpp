#include "light.h"

Light::Light(XMFLOAT3 strength) : m_strength{ strength }
{
}

void Light::SetStrength(XMFLOAT3 strength)
{
	m_strength = strength;
}

DirectionalLight::DirectionalLight(XMFLOAT3 strength, XMFLOAT3 direction) : Light(strength), m_direction{ direction }
{
}

void DirectionalLight::UpdateShaderVariable(DirectionalLightData& buffer)
{
	buffer.strength = m_strength;
	buffer.direction = m_direction;
}

void DirectionalLight::SetDirection(XMFLOAT3 direction)
{
	m_direction = direction;
}

PointLight::PointLight(XMFLOAT3 strength, XMFLOAT3 position, FLOAT fallOffStart, FLOAT fallOffEnd) :
	Light(strength), m_position {position}, m_fallOffStart{fallOffStart}, m_fallOffEnd{fallOffEnd}
{
}

void PointLight::UpdateShaderVariable(PointLightData& buffer)
{
	buffer.strength = m_strength;
	buffer.position = m_position;
	buffer.fallOffStart = m_fallOffStart;
	buffer.fallOffEnd = m_fallOffEnd;
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

SpotLight::SpotLight(XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position, 
	FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower) :
	Light(strength), m_direction{direction}, m_position{position},
	m_fallOffStart{fallOffStart}, m_fallOffEnd{fallOffEnd}, m_spotPower{spotPower}
{
}

void SpotLight::UpdateShaderVariable(SpotLightData& buffer)
{
	buffer.strength = m_strength;
	buffer.direction = m_direction;
	buffer.position = m_position;
	buffer.fallOffStart = m_fallOffStart;
	buffer.fallOffEnd = m_fallOffEnd;
	buffer.spotPower = m_spotPower;
}

void SpotLight::SetDirection(XMFLOAT3 direction)
{
	m_direction = direction;
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

LightSystem::LightSystem(const ComPtr<ID3D12Device>& device) : m_lightNum{ 0, 0, 0, 0 }
{
	m_constantBuffer = make_unique<UploadBuffer<LightData>>(device, (UINT)RootParameter::Light);
}

void LightSystem::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	LightData buffer;
	buffer.lightNum = m_lightNum;
	for (int i = 0; const auto& directionalLight : m_directionalLights) {
		directionalLight->UpdateShaderVariable(buffer.directionalLights[i++]); }
	for (int i = 0; const auto& pointLight : m_pointLights) {
		pointLight->UpdateShaderVariable(buffer.pointLights[i++]); }
	for (int i = 0; const auto& spotLight : m_spotLights) {
		spotLight->UpdateShaderVariable(buffer.spotLights[i++]); }
	m_constantBuffer->Copy(buffer);

	m_constantBuffer->UpdateRootConstantBuffer(commandList);
}

void LightSystem::SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight)
{
	if (m_directionalLights.size() == Settings::MaxDirectionalLight) assert("");
	m_directionalLights.push_back(directionalLight);
	m_lightNum.x = static_cast<UINT>(m_directionalLights.size());
}

void LightSystem::SetPointLight(shared_ptr<PointLight> pointLight)
{
	if (m_pointLights.size() == Settings::MaxPointLight) assert("");
	m_pointLights.push_back(pointLight);
	m_lightNum.y = static_cast<UINT>(m_pointLights.size());
}

void LightSystem::SetSpotLight(shared_ptr<SpotLight> spotLight)
{
	if (m_spotLights.size() == Settings::MaxSpotLight) assert("");
	m_spotLights.push_back(spotLight);
	m_lightNum.z = static_cast<UINT>(m_spotLights.size());
}
