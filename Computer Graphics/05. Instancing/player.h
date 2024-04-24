#pragma once
#include "object.h"
#include "camera.h"

class Player : public InstanceObject
{
public:
	Player();
	~Player() override = default;

	void MouseEvent(FLOAT timeElapsed);
	void KeyboardEvent(FLOAT timeElapsed);
	virtual void Update(FLOAT timeElapsed) override;

	void SetCamera(const shared_ptr<Camera>& camera);

private:
	shared_ptr<Camera> m_camera;

	FLOAT m_speed;
};