#include "scene.h"
#include "stdafx.h"
#include "framework.h"

Scene::Scene()
{
}

void Scene::MouseEvent(HWND hWnd, FLOAT timeElapsed)
{
	SetCursor(NULL);
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);

	POINT lastMousePosition{ 
		windowRect.left + static_cast<LONG>(g_framework->GetWindowWidth() / 2), 
		windowRect.top + static_cast<LONG>(g_framework->GetWindowWidth() / 2) };
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	float dx = XMConvertToRadians(0.15f * static_cast<FLOAT>(mousePosition.x - lastMousePosition.x));
	float dy = XMConvertToRadians(0.15f * static_cast<FLOAT>(mousePosition.y - lastMousePosition.y));

	if (m_camera) {
		m_camera->RotateYaw(dx);
		m_camera->RotatePitch(dy);
	}
	SetCursorPos(lastMousePosition.x, lastMousePosition.y);

	m_player->MouseEvent(timeElapsed);
}

void Scene::KeyboardEvent(FLOAT timeElapsed)
{
	m_player->KeyboardEvent(timeElapsed);
}


void Scene::Update(FLOAT timeElapsed)
{
	m_player->Update(timeElapsed);
	for (auto& object : m_objects) {
		object->Update(timeElapsed);
	}
	m_skybox->SetPosition(m_camera->GetEye());
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_camera->UpdateShaderVariable(commandList);
	m_shaders.at("OBJECT")->UpdateShaderVariable(commandList);
	for (auto& object : m_objects) {
		object->Render(commandList);
	}
	m_player->Render(commandList);

	m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
	m_skybox->Render(commandList);
}

void Scene::BuildObjects(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	auto objectShader = make_shared<ObjectShader>(device, rootSignature);
	m_shaders.insert({ "OBJECT", objectShader });
	auto skyboxShader = make_shared<SkyboxShader>(device, rootSignature);
	m_shaders.insert({ "SKYBOX", skyboxShader });

	auto cube = make_shared<CubeMesh>(device, commandList);
	m_meshes.insert({ "CUBE", cube });
	auto skyboxMesh = make_shared<SkyboxMesh>(device, commandList);
	m_meshes.insert({ "SKYBOX", skyboxMesh });

	auto checkboardTexture = make_shared<Texture>(device, commandList, 
		TEXT("Textures/Checkboard.dds"), RootParameter::Texture);
	m_textures.insert({ "TEXTURE1", checkboardTexture });
	auto brickTextire = make_shared<Texture>(device, commandList, 
		TEXT("Textures/Brick.dds"), RootParameter::Texture);
	m_textures.insert({ "TEXTURE2", brickTextire });
	auto skyboxTexture = make_shared<Texture>(device, commandList,
		TEXT("Textures/Skybox.dds"), RootParameter::TextureCube);
	m_textures.insert({ "SKYBOX", skyboxTexture });

	m_player = make_shared<Player>();
	m_player->SetMesh(cube);
	m_player->SetTexture(checkboardTexture);
	m_player->SetPosition(XMFLOAT3{ 0.f, 0.f, 0.f });

	for (int x = -15; x <= 15; x += 5) {
		for (int y = -15; y <= 15; y += 5) {
			for (int z = -15; z <= 15; z += 5) {
				auto object = make_shared<RotatingObject>();
				object->SetMesh(cube);
				object->SetTexture(brickTextire);
				object->SetPosition(XMFLOAT3{ 
					static_cast<FLOAT>(x), 
					static_cast<FLOAT>(y), 
					static_cast<FLOAT>(z) });
				m_objects.push_back(object);
			}
		}
	}

	m_camera = make_shared<ThirdPersonCamera>();
	m_camera->SetLens(0.25 * XM_PI, g_framework->GetAspectRatio(), 0.1f, 1000.f);
	m_player->SetCamera(m_camera);

	m_skybox = make_shared<GameObject>();
	m_skybox->SetMesh(skyboxMesh);
	m_skybox->SetTexture(skyboxTexture);
}

void Scene::ReleaseUploadBuffer()
{
	for (auto& [name, mesh] : m_meshes) {
		mesh->ReleaseUploadBuffer();
	}
	for (auto& [name, texture] : m_textures) {
		texture->ReleaseUploadBuffer();
	}
}

void Scene::MouseEvent(UINT message, LPARAM lParam)
{
}

void Scene::KeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}