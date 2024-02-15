#pragma once
#include "object.h"
#include "camera.h"

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

	void MouseEvent(FLOAT timeElapsed);
	void KeyboardEvent(FLOAT timeElapsed);
	virtual void Update(FLOAT timeElapsed) override;
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;

	void SetCamera(const shared_ptr<Camera>& camera);

private:
	shared_ptr<Camera> m_camera;

	FLOAT m_speed;
};

