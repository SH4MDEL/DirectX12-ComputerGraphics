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

	XMFLOAT3 m_eye;		// ī�޶� ��ġ
	XMFLOAT3 m_at;		// ī�޶� �ٶ󺸴� ����
	XMFLOAT3 m_up;		// �뷫���� ��

	XMFLOAT3 m_u;		// ī�޶� ���� x�� ����
	XMFLOAT3 m_v;		// ī�޶� ���� y�� ����
	XMFLOAT3 m_n;		// ī�޶� ���� z�� ����
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
