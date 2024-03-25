#pragma once
#include "stdafx.h"
#include "buffer.h"

struct CameraData : public BufferBase
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT3 eye;
};

class Camera
{
public:
	Camera(const ComPtr<ID3D12Device>& device);
	~Camera() = default;

	virtual void Update(FLOAT timeElapsed) = 0;
	virtual void UpdateEye(XMFLOAT3 position) = 0;
	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

	virtual void RotatePitch(FLOAT radian) = 0;
	virtual void RotateYaw(FLOAT radian) = 0;

	void SetLens(FLOAT fovy, FLOAT aspect, FLOAT minZ, FLOAT maxZ);

	XMFLOAT3 GetEye() const;
	XMFLOAT3 GetU() const;
	XMFLOAT3 GetV() const;
	XMFLOAT3 GetN() const;

protected:
	void UpdateBasis();

protected:
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;

	XMFLOAT3 m_eye;
	XMFLOAT3 m_at;
	XMFLOAT3 m_up;

	XMFLOAT3 m_u;
	XMFLOAT3 m_v;
	XMFLOAT3 m_n;

	unique_ptr<UploadBuffer<CameraData>> m_bufferPointer;
};

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera(const ComPtr<ID3D12Device>& device);
	~ThirdPersonCamera() = default;

	void Update(FLOAT timeElapsed) override;
	void UpdateEye(XMFLOAT3 position) override;

	void RotatePitch(FLOAT radian) override;
	void RotateYaw(FLOAT radian) override;
private:
	FLOAT m_radius;
	FLOAT m_phi;
	FLOAT m_theta;
};
