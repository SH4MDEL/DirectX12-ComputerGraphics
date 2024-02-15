#pragma once
#include "stdafx.h"

class Camera
{
public:
	Camera();
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

	XMFLOAT3 m_eye;		// 카메라 위치
	XMFLOAT3 m_at;		// 카메라가 바라보는 방향
	XMFLOAT3 m_up;		// 대략적인 위

	XMFLOAT3 m_u;		// 카메라 공간 x축 기저
	XMFLOAT3 m_v;		// 카메라 공간 y축 기저
	XMFLOAT3 m_n;		// 카메라 공간 z축 기저
};

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera();
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
