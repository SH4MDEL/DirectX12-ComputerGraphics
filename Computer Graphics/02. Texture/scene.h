#pragma once
#include "stdafx.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "object.h"
#include "player.h"
#include "camera.h"

class Scene
{
public:
	Scene();
	~Scene() = default;

	void MouseEvent(HWND hWnd, FLOAT timeElapsed);
	void KeyboardEvent(FLOAT timeElapsed);
	void Update(FLOAT timeElapsed);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void BuildObjects(const ComPtr<ID3D12Device>& device, 
		const ComPtr<ID3D12GraphicsCommandList>& commandList, 
		const ComPtr<ID3D12RootSignature>& rootSignature);
	void ReleaseUploadBuffer();

	void MouseEvent(UINT message, LPARAM lParam);
	void KeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	unordered_map<string, shared_ptr<Shader>> m_shaders;
	unordered_map<string, shared_ptr<Mesh>> m_meshes;
	unordered_map<string, shared_ptr<Texture>> m_textures;

	shared_ptr<Camera> m_camera;
	shared_ptr<Player> m_player;
	vector<shared_ptr<GameObject>> m_objects;
	shared_ptr<GameObject> m_skybox;
};