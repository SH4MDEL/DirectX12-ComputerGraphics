#include "light.h"

Light::Light(const ComPtr<ID3D12Device>& device) : m_strength{ 1.f, 1.f, 1.f }
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
	m_constantBuffer = make_unique<UploadBuffer<ShadowData>>(device, (UINT)RootParameter::Shadow);
}

Light::Light(const ComPtr<ID3D12Device>& device, XMFLOAT3 strength) : m_strength{ strength }
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
	m_constantBuffer = make_unique<UploadBuffer<ShadowData>>(device, (UINT)RootParameter::Shadow);
}

void Light::SetStrength(XMFLOAT3 strength)
{
	m_strength = strength;
}

void Light::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	ShadowData buffer;

	XMStoreFloat4x4(&buffer.lightViewMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&m_viewMatrix)));
	XMStoreFloat4x4(&buffer.lightProjectionMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix)));

	m_constantBuffer->Copy(buffer);
	m_constantBuffer->UpdateRootConstantBuffer(commandList);
}

DirectionalLight::DirectionalLight(const ComPtr<ID3D12Device>& device) : Light(device), 
	m_direction{ 0.f, -1.f, 0.f }
{
}

DirectionalLight::DirectionalLight(const ComPtr<ID3D12Device>& device,
	XMFLOAT3 strength, XMFLOAT3 direction) : Light(device, strength), m_direction{ direction }
{
	m_direction = Utiles::Vector3::Normalize(m_direction);
}

void DirectionalLight::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, 
	DirectionalLightData& buffer)
{
	BoundingSphere sceneSphere{ XMFLOAT3{ 0.0f, 0.0f, 0.0f }, 128.5f * sqrt(2.f) };

	XMFLOAT3 lightPos{ Utiles::Vector3::Mul(m_direction, -2.0f * sceneSphere.Radius) };
	XMFLOAT3 targetPos{ sceneSphere.Center };
	XMFLOAT3 lightUp{ 0.0f, 1.0f, 0.0f };

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&lightPos),
		XMLoadFloat3(&targetPos), XMLoadFloat3(&lightUp)));

	XMFLOAT3 sphereCenterLS{ Utiles::Vector3::TransformCoord(targetPos, m_viewMatrix) };

	float l = sphereCenterLS.x - sceneSphere.Radius;
	float b = sphereCenterLS.y - sceneSphere.Radius;
	float n = sphereCenterLS.z - sceneSphere.Radius;
	float r = sphereCenterLS.x + sceneSphere.Radius;
	float t = sphereCenterLS.y + sceneSphere.Radius;
	float f = sphereCenterLS.z + sceneSphere.Radius;

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f));

	Light::UpdateShaderVariable(commandList);
	buffer.strength = m_strength;
	buffer.direction = m_direction;
}

void DirectionalLight::SetDirection(XMFLOAT3 direction)
{
	m_direction = Utiles::Vector3::Normalize(direction);
}

PointLight::PointLight(const ComPtr<ID3D12Device>& device) : Light(device), 
	m_position{ 0.f, 0.f, 0.f }, m_fallOffStart{ 0.1f }, m_fallOffEnd{ 10.f }
{
}

PointLight::PointLight(const ComPtr<ID3D12Device>& device, 
	XMFLOAT3 strength, XMFLOAT3 position, FLOAT fallOffStart, FLOAT fallOffEnd) :
	Light(device, strength), m_position {position}, m_fallOffStart{fallOffStart}, m_fallOffEnd{fallOffEnd}
{
}

void PointLight::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, 
	PointLightData& buffer)
{
	//Light::UpdateShaderVariable(commandList);
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

SpotLight::SpotLight(const ComPtr<ID3D12Device>& device) :
	Light(device), m_direction{ 0.f, 1.f, 0.f }, m_position{ 0.f, 0.f, 0.f },
	m_fallOffStart{ 0.1f }, m_fallOffEnd{ 10.f }, m_spotPower{ 10.f }
{
	m_direction = Utiles::Vector3::Normalize(m_direction);
}

SpotLight::SpotLight(const ComPtr<ID3D12Device>& device, 
	XMFLOAT3 strength, XMFLOAT3 direction, XMFLOAT3 position,
	FLOAT fallOffStart, FLOAT fallOffEnd, FLOAT spotPower) :
	Light(device, strength), m_direction{direction}, m_position{position},
	m_fallOffStart{fallOffStart}, m_fallOffEnd{fallOffEnd}, m_spotPower{spotPower}
{
	m_direction = Utiles::Vector3::Normalize(m_direction);
}

void SpotLight::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, 
	SpotLightData& buffer)
{
	//Light::UpdateShaderVariable(commandList);
	buffer.strength = m_strength;
	buffer.direction = m_direction;
	buffer.position = m_position;
	buffer.fallOffStart = m_fallOffStart;
	buffer.fallOffEnd = m_fallOffEnd;
	buffer.spotPower = m_spotPower;
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

LightSystem::LightSystem(const ComPtr<ID3D12Device>& device) : m_lightNum{ 0, 0, 0, 0 }
{
	m_constantBuffer = make_unique<UploadBuffer<LightData>>(device, (UINT)RootParameter::Light);
}

void LightSystem::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	LightData buffer;
	buffer.lightNum = m_lightNum;
	for (int i = 0; const auto& directionalLight : m_directionalLights) {
		directionalLight->UpdateShaderVariable(commandList, buffer.directionalLights[i++]); }
	for (int i = 0; const auto& pointLight : m_pointLights) {
		pointLight->UpdateShaderVariable(commandList, buffer.pointLights[i++]); }
	for (int i = 0; const auto& spotLight : m_spotLights) {
		spotLight->UpdateShaderVariable(commandList, buffer.spotLights[i++]); }
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
