#include "object.h"

GameObject::GameObject() : m_right{1.f, 0.f, 0.f}, m_up{0.f, 1.f, 0.f}, m_front{0.f, 0.f, 1.f}
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
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
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(RootParameter::GameObject, 16, &worldMatrix, 0);

	if (m_texture) m_texture->UpdateShaderVariable(commandList);
}

void GameObject::Transform(XMFLOAT3 shift)
{
	SetPosition(Utiles::Vector3::Add(GetPosition(), shift));
}

void GameObject::Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4(&m_worldMatrix, rotate * XMLoadFloat4x4(&m_worldMatrix));

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void GameObject::SetMesh(const shared_ptr<MeshBase>& mesh)
{
	m_mesh = mesh;
}

void GameObject::SetTexture(const shared_ptr<Texture>& texture)
{
	m_texture = texture;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3{m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43};
}

RotatingObject::RotatingObject() : GameObject(), m_rotatingSpeed{ Utiles::Random::GetFloat(10.f, 50.f) }
{
}

void RotatingObject::Update(FLOAT timeElapsed)
{
	Rotate(0.f, m_rotatingSpeed * timeElapsed, 0.f);
}

Terrain::Terrain() : GameObject()
{
}

FLOAT Terrain::GetHeight(FLOAT x, FLOAT z)
{
	const XMFLOAT3 position = GetPosition();
	return static_pointer_cast<TerrainMesh>(m_mesh)->
		GetHeight(x - position.x, z - position.z) + position.y + 0.3f;
}
