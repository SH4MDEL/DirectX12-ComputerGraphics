#pragma once
#include "stdafx.h"
#include "mesh.h"
#include "texture.h"
#include "buffer.h"

class InstanceObject
{
public:
	InstanceObject(const ComPtr<ID3D12Device>& device);
	virtual ~InstanceObject() = default;

	virtual void Update(FLOAT timeElapsed);

	void Transform(XMFLOAT3 shift);
	void Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll);

	void SetPosition(XMFLOAT3 position);
	void SetTextureIndex(UINT textureIndex);

	XMFLOAT3 GetPosition() const;
	UINT GetTextureIndex() const;
	XMFLOAT4X4 GetWorldMatrix() const;

protected:
	XMFLOAT4X4			m_worldMatrix;

	XMFLOAT3			m_right;
	XMFLOAT3			m_up;
	XMFLOAT3			m_front;

	UINT				m_textureIndex;
};

struct ObjectData : public BufferBase
{
	XMFLOAT4X4 worldMatrix;
};

class GameObject : public InstanceObject
{
public:
	GameObject(const ComPtr<ID3D12Device>& device);
	virtual ~GameObject() = default;

	virtual void Update(FLOAT timeElapsed) override;
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetMesh(const shared_ptr<MeshBase>& mesh);
	void SetTexture(const shared_ptr<Texture>& texture);

protected:
	shared_ptr<MeshBase>	m_mesh;
	shared_ptr<Texture>		m_texture;

	unique_ptr<UploadBuffer<ObjectData>> m_constantBuffer;
};

class RotatingObject : public InstanceObject
{
public:
	RotatingObject(const ComPtr<ID3D12Device>& device);
	~RotatingObject() override = default;

	void Update(FLOAT timeElapsed) override;

private:
	FLOAT m_rotatingSpeed;
};

class Terrain : public GameObject
{
public:
	Terrain(const ComPtr<ID3D12Device>& device);
	~Terrain() override = default;

	FLOAT GetHeight(FLOAT x, FLOAT z);
};