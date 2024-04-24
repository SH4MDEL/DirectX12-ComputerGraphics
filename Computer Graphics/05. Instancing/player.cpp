#include "player.h"

Player::Player() :
	InstanceObject(), m_speed{ Settings::PlayerSpeed }
{
}

void Player::MouseEvent(FLOAT timeElapsed)
{
}

void Player::KeyboardEvent(FLOAT timeElapsed)
{
	XMFLOAT3 front{ m_camera->GetN() }; front.y = 0.f;
	front = Utiles::Vector3::Normalize(front);
	XMFLOAT3 back{ Utiles::Vector3::Negate(front) };
	XMFLOAT3 right{ m_camera->GetU() };
	XMFLOAT3 left{ Utiles::Vector3::Negate(right) };
	XMFLOAT3 direction{};

	if (GetAsyncKeyState('R')) {
		auto position = GetPosition();
		position.y += m_speed * timeElapsed;
		SetPosition(position);
	}
	if (GetAsyncKeyState('F')) {
		auto position = GetPosition();
		position.y -= m_speed * timeElapsed;
		SetPosition(position);
	}

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
	if (GetAsyncKeyState('W') || GetAsyncKeyState('A') ||
		GetAsyncKeyState('S') || (GetAsyncKeyState('D') & 0x8000)) {
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

void Player::SetCamera(const shared_ptr<Camera>& camera)
{
	m_camera = camera;
}
