#pragma once
#include "stdafx.h"
#include "mesh.h"
#include "texture.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() = default;

	virtual void Update(FLOAT timeElapsed);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void Transform(XMFLOAT3 shift);
	void Rotate(FLOAT pitch, FLOAT yaw, FLOAT roll);

	void SetMesh(const shared_ptr<MeshBase>& mesh);
	void SetTexture(const shared_ptr<Texture>& texture);

	void SetPosition(XMFLOAT3 position);
	XMFLOAT3 GetPosition() const;

protected:
	XMFLOAT4X4			m_worldMatrix;

	XMFLOAT3			m_right;
	XMFLOAT3			m_up;
	XMFLOAT3			m_front;

	shared_ptr<MeshBase>	m_mesh;
	shared_ptr<Texture>	m_texture;
};

class RotatingObject : public GameObject
{
public:
	RotatingObject();
	~RotatingObject() override = default;

	void Update(FLOAT timeElapsed) override;

private:
	FLOAT m_rotatingSpeed;
};

class Terrain : public GameObject
{
public:
	Terrain();
	~Terrain() override = default;

	FLOAT GetHeight(FLOAT x, FLOAT z);
};