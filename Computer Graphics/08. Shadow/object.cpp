#include "object.h"
#include "Instance.h"

Object::Object() :
	m_right{ 1.f, 0.f, 0.f }, m_up{ 0.f, 1.f, 0.f }, m_front{ 0.f, 0.f, 1.f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

void Object::Transform(XMFLOAT3 shift)
{
	SetPosition(Utiles::Vector3::Add(GetPosition(), shift));
}

void Object::Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4(&m_worldMatrix, rotate * XMLoadFloat4x4(&m_worldMatrix));

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void Object::SetPosition(XMFLOAT3 position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

XMFLOAT3 Object::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

InstanceObject::InstanceObject() : Object(),  
	m_textureIndex{ 0 }, m_materialIndex{ 0 }
{

}

void InstanceObject::Update(FLOAT timeElapsed)
{
}

void InstanceObject::UpdateShaderVariable(InstanceData& buffer)
{
	XMStoreFloat4x4(&buffer.worldMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	buffer.textureIndex = m_textureIndex;
	buffer.materialIndex = m_materialIndex;
}

void InstanceObject::SetTextureIndex(UINT textureIndex)
{
	m_textureIndex = textureIndex;
}

void InstanceObject::SetMaterialIndex(UINT materialIndex)
{
	m_materialIndex = materialIndex;
}

GameObject::GameObject(const ComPtr<ID3D12Device>& device) : Object()
{
	m_constantBuffer = make_unique<UploadBuffer<ObjectData>>(device, (UINT)RootParameter::GameObject);
}

void GameObject::Update(FLOAT timeElapsed)
{
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	UpdateShaderVariable(commandList);
	m_mesh->Render(commandList);
}

void GameObject::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	ObjectData buffer;
	XMStoreFloat4x4(&buffer.worldMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	m_constantBuffer->Copy(buffer);

	m_constantBuffer->UpdateRootConstantBuffer(commandList);
	if (m_texture) m_texture->UpdateShaderVariable(commandList);
	if (m_material) m_material->UpdateShaderVariable(commandList);
}

void GameObject::SetMesh(const shared_ptr<MeshBase>& mesh)
{
	m_mesh = mesh;
}

void GameObject::SetTexture(const shared_ptr<Texture>& texture)
{
	m_texture = texture;
}

void GameObject::SetMaterial(const shared_ptr<Material>& material)
{
	m_material = material;
}

RotatingObject::RotatingObject() : 
	InstanceObject(), m_rotatingSpeed{ Utiles::Random::GetFloat(10.f, 50.f) }
{
}

void RotatingObject::Update(FLOAT timeElapsed)
{
	Rotate(0.f, m_rotatingSpeed * timeElapsed, 0.f);
}

Terrain::Terrain(const ComPtr<ID3D12Device>& device) : 
	GameObject(device)
{
}

FLOAT Terrain::GetHeight(FLOAT x, FLOAT z)
{
	const XMFLOAT3 position = GetPosition();
	return static_pointer_cast<TerrainMesh>(m_mesh)->
		GetHeight(x - position.x, z - position.z) + position.y + 0.3f;
}

LightObject::LightObject(const shared_ptr<SpotLight>& light) : 
	RotatingObject(), m_light{light}
{
}

void LightObject::Update(FLOAT timeElapsed)
{
	RotatingObject::Update(timeElapsed);
	m_light->SetDirection(m_front);
	m_light->SetPosition(GetPosition());
}

Sun::Sun(const shared_ptr<DirectionalLight>& light) : m_light{ light },
m_strength{ 1.f, 1.f, 1.f }, m_phi{ XM_1DIV2PI + 0.5f }, m_theta{ XM_PIDIV4 }, m_radius{ Settings::SunRadius }
{
}

void Sun::SetStrength(XMFLOAT3 strength)
{
	m_strength = strength;
}

void Sun::Update(FLOAT timeElapsed)
{
	//m_phi += timeElapsed * 0.4f;

	FLOAT cosine = cos(m_phi);
	XMFLOAT3 offset{
		m_radius * sin(m_phi) * cos(m_theta),
		m_radius * cosine,
		m_radius * sin(m_phi) * sin(m_theta) };

	SetPosition(offset);
	m_light->SetDirection(Utiles::Vector3::Negate(GetPosition()));
	m_light->SetStrength(cosine > 0.f ?
		Utiles::Vector3::Mul(m_strength, cosine) : XMFLOAT3{ 0.f,0.f,0.f });
}