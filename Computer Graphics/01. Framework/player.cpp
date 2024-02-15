#include "player.h"

Player::Player() : GameObject(), m_speed{Settings::PlayerSpeed}
{
}

void Player::MouseEvent(FLOAT timeElapsed)
{
}

void Player::KeyboardEvent(FLOAT timeElapsed)
{
	XMFLOAT3 front{ m_camera->GetN() }; front.y = 0.f; 
	front = Utiles::Vector3::Normalize(front);
	XMFLOAT3 back{ Utiles::Vector3::Sub(XMFLOAT3{}, m_camera->GetN()) }; back.y = 0.f; 
	back = Utiles::Vector3::Normalize(back);
	XMFLOAT3 left{ Utiles::Vector3::Sub(XMFLOAT3{}, m_camera->GetU()) };
	XMFLOAT3 right{ m_camera->GetU() };
	XMFLOAT3 direction{};

	if (GetAsyncKeyState('W') && GetAsyncKeyState('A') & 0x8000) {
		direction = Utiles::Vector3::Normalize(Utiles::Vector3::Add(front, left));
	}
	else if (GetAsyncKeyState('W') && GetAsyncKeyState('D') & 0x8000) {
		direction = Utiles::Vector3::Normalize(Utiles::Vector3::Add(front, right));
	}
	else if (GetAsyncKeyState('S') && GetAsyncKeyState('A') & 0x8000) {
		direction = Utiles::Vector3::Normalize(Utiles::Vector3::Add(back, left));
	}
	else if (GetAsyncKeyState('S') && GetAsyncKeyState('D') & 0x8000) {
		direction = Utiles::Vector3::Normalize(Utiles::Vector3::Add(back, right));
	}
	else if (GetAsyncKeyState('W') & 0x8000) {
		direction = front;
	}
	else if (GetAsyncKeyState('A') & 0x8000) {
		direction = left;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		direction = back;
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		direction = right;
	}
	if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState('A') & 0x8000) ||
		(GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) {
		XMFLOAT3 angle{ Utiles::Vector3::Angle(m_front, direction) };
		XMFLOAT3 cross{ Utiles::Vector3::Cross(m_front, direction) };
		if (cross.y >= 0.f) {
			Rotate(0.f, XMConvertToDegrees(angle.y) * 10.f * timeElapsed, 0.f);
		}
		else {
			Rotate(0.f, -XMConvertToDegrees(angle.y) * 10.f * timeElapsed, 0.f);
		}
		Transform(Utiles::Vector3::Mul(m_front, m_speed * timeElapsed));
	}
}


void Player::Update(FLOAT timeElapsed)
{
	if (m_camera) m_camera->UpdateEye(GetPosition());
}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	Player::UpdateShaderVariable(commandList);
	m_mesh->Render(commandList);
}

void Player::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	GameObject::UpdateShaderVariable(commandList);
	if (m_camera) m_camera->UpdateShaderVariable(commandList);
}

void Player::SetCamera(const shared_ptr<Camera>& camera)
{
	m_camera = camera;
}
