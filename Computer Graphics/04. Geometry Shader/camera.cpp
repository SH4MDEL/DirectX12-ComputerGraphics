#include "camera.h"

Camera::Camera() : m_eye{0.f, 0.f, 0.f}, m_at{0.f, 0.f, 1.f}, m_up{0.f, 1.f, 0.f},
	m_u{1.f, 0.f, 0.f}, m_v{0.f, 1.f, 0.f}, m_n{0.f, 0.f, 1.f}
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
}

void Camera::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&m_at), XMLoadFloat3(&m_up)));

	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_viewMatrix)));
	commandList->SetGraphicsRoot32BitConstants(RootParameter::Camera, 16, &viewMatrix, 0);

	XMFLOAT4X4 projectionMatrix;
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix)));
	commandList->SetGraphicsRoot32BitConstants(RootParameter::Camera, 16, &projectionMatrix, 16);

	commandList->SetGraphicsRoot32BitConstants(RootParameter::Camera, 3, &m_eye, 32);
}

void Camera::SetLens(FLOAT fovy, FLOAT aspect, FLOAT minZ, FLOAT maxZ)
{
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fovy, aspect, minZ, maxZ));
}

XMFLOAT3 Camera::GetEye() const
{
	return m_eye;
}

XMFLOAT3 Camera::GetU() const
{
	return m_u;
}

XMFLOAT3 Camera::GetV() const
{
	return m_v;
}

XMFLOAT3 Camera::GetN() const
{
	return m_n;
}

void Camera::UpdateBasis()
{
	m_n = Utiles::Vector3::Normalize(Utiles::Vector3::Sub(m_at, m_eye));
	m_u = Utiles::Vector3::Normalize(Utiles::Vector3::Cross(m_up, m_n));
	m_v = Utiles::Vector3::Normalize(Utiles::Vector3::Cross(m_n, m_u));
}

ThirdPersonCamera::ThirdPersonCamera() : Camera{}, m_radius{Settings::DefaultCameraRadius},
	m_phi{Settings::DefaultCameraPitch}, m_theta{Settings::DefaultCameraYaw}
{

}

void ThirdPersonCamera::Update(FLOAT timeElapsed)
{

}

void ThirdPersonCamera::UpdateEye(XMFLOAT3 position)
{
	XMFLOAT3 offset{
	m_radius * sin(m_phi) * cos(m_theta),
	m_radius * cos(m_phi),
	m_radius * sin(m_phi) * sin(m_theta) };

	m_eye = Utiles::Vector3::Add(position, offset);
	m_at = position;
	UpdateBasis();
}

void ThirdPersonCamera::RotatePitch(FLOAT radian)
{
	m_phi += radian;
	m_phi = clamp(m_phi, Settings::CameraMinPitch, Settings::CameraMaxPitch);
}

void ThirdPersonCamera::RotateYaw(FLOAT radian)
{
	m_theta += radian;
}
