#include "object.h"

InstanceObject::InstanceObject(const ComPtr<ID3D12Device>& device) :
	m_right{ 1.f, 0.f, 0.f }, m_up{ 0.f, 1.f, 0.f }, m_front{ 0.f, 0.f, 1.f }, m_textureIndex{ 0 }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

void InstanceObject::Update(FLOAT timeElapsed)
{
}

void InstanceObject::Transform(XMFLOAT3 shift)
{
	SetPosition(Utiles::Vector3::Add(GetPosition(), shift));
}

void InstanceObject::Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4(&m_worldMatrix, rotate * XMLoadFloat4x4(&m_worldMatrix));

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void InstanceObject::SetPosition(XMFLOAT3 position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

void InstanceObject::SetTextureIndex(UINT textureIndex)
{
	m_textureIndex = textureIndex;
}

void InstanceObject::SetMaterialIndex(UINT materialIndex)
{
	m_materialIndex = materialIndex;
}

XMFLOAT3 InstanceObject::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

UINT InstanceObject::GetTextureIndex() const
{
	return m_textureIndex;
}

UINT InstanceObject::GetMaterialIndex() const
{
	return m_materialIndex;
}

XMFLOAT4X4 InstanceObject::GetWorldMatrix() const
{
	return m_worldMatrix;
}

GameObject::GameObject(const ComPtr<ID3D12Device>& device) : InstanceObject(device)
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

RotatingObject::RotatingObject(const ComPtr<ID3D12Device>& device) : 
	InstanceObject(device), m_rotatingSpeed{ Utiles::Random::GetFloat(10.f, 50.f) }
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

