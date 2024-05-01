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
		windowRect.top + static_cast<LONG>(g_framework->GetWindowHeight() / 2) };
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
	m_sun->Update(timeElapsed);

	for (auto& object : m_objects) {
		object->Update(timeElapsed);
	}
	m_skybox->SetPosition(m_camera->GetEye());
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_camera->UpdateShaderVariable(commandList);
	m_lightSystem->UpdateShaderVariable(commandList);

	m_shaders.at("OBJECT")->UpdateShaderVariable(commandList);
	m_instanceObject->Render(commandList);

	m_shaders.at("DETAIL")->UpdateShaderVariable(commandList);
	m_terrain->Render(commandList);

	m_shaders.at("BILLBOARD")->UpdateShaderVariable(commandList);
	m_instanceBillboard->Render(commandList);

	m_shaders.at("SKYBOX")->UpdateShaderVariable(commandList);
	m_skybox->Render(commandList);
}

void Scene::BuildObjects(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	BuildShaders(device, commandList, rootSignature);
	BuildMeshes(device, commandList);
	BuildTextures(device, commandList);
	BuildMaterials(device, commandList);

	BuildObjects(device);
}

inline void Scene::BuildShaders(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const ComPtr<ID3D12RootSignature>& rootSignature)
{
	auto objectShader = make_shared<ObjectShader>(device, rootSignature);
	m_shaders.insert({ "OBJECT", objectShader });
	auto skyboxShader = make_shared<SkyboxShader>(device, rootSignature);
	m_shaders.insert({ "SKYBOX", skyboxShader });
	auto terrainShader = make_shared<TerrainShader>(device, rootSignature);
	m_shaders.insert({ "DETAIL", terrainShader });
	auto billboardShader = make_shared<BillboardShader>(device, rootSignature);
	m_shaders.insert({ "BILLBOARD", billboardShader });
}

inline void Scene::BuildMeshes(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeMesh = make_shared<Mesh<TextureVertex>>(device, commandList,
		TEXT("../Resources/Meshes/CubeNormalMesh.binary"));
	m_meshes.insert({ "CUBE", cubeMesh });
	auto skyboxMesh = make_shared<Mesh<Vertex>>(device, commandList,
		TEXT("../Resources/Meshes/SkyboxMesh.binary"));
	m_meshes.insert({ "SKYBOX", skyboxMesh });
	auto terrainMesh = make_shared<TerrainMesh>(device, commandList,
		TEXT("../Resources/Terrain/HeightMap.binary"));
	m_meshes.insert({ "TERRAIN", terrainMesh });
	auto billboardMesh = make_shared<Mesh<TextureVertex>>(device, commandList,
		TEXT("../Resources/Meshes/billboardMesh.binary"), D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_meshes.insert({ "BILLBOARD", billboardMesh });
}

inline void Scene::BuildTextures(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeTexture = make_shared<Texture>(device);
	cubeTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/Checkboard.dds"), RootParameter::Texture);
	cubeTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/Brick.dds"), RootParameter::Texture);
	cubeTexture->CreateShaderVariable(device);
	m_textures.insert({ "CUBE", cubeTexture });

	auto skyboxTexture = make_shared<Texture>(device, commandList,
		TEXT("../Resources/Textures/Skybox.dds"), RootParameter::TextureCube);
	m_textures.insert({ "SKYBOX", skyboxTexture });

	auto terrainTexture = make_shared<Texture>(device);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/TerrainBase.dds"), RootParameter::Texture);
	terrainTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/TerrainDetail.dds"), RootParameter::Texture);
	terrainTexture->CreateShaderVariable(device);
	m_textures.insert({ "TERRAIN", terrainTexture });

	auto grassTexture = make_shared<Texture>(device);
	grassTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/Grass01.dds"), RootParameter::Texture);
	grassTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/Grass02.dds"), RootParameter::Texture);
	grassTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/Grass03.dds"), RootParameter::Texture);
	grassTexture->LoadTexture(device, commandList,
		TEXT("../Resources/Textures/Grass04.dds"), RootParameter::Texture);
	grassTexture->CreateShaderVariable(device);
	m_textures.insert({ "GRASS", grassTexture });
}

inline void Scene::BuildMaterials(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto cubeMaterial = make_shared<Material>();
	cubeMaterial->SetMaterial(XMFLOAT3{0.95f, 0.93f, 0.88f}, 0.125f, XMFLOAT3{ 0.1f, 0.1f, 0.1f });
	cubeMaterial->CreateShaderVariable(device);
	m_materials.insert({ "CUBE", cubeMaterial });

	auto terrainMaterial = make_shared<Material>();
	terrainMaterial->SetMaterial(XMFLOAT3{ 0.01f, 0.01f, 0.01f }, 0.9f, XMFLOAT3{ 0.3f, 0.3f, 0.3f });
	terrainMaterial->CreateShaderVariable(device);
	m_materials.insert({ "TERRAIN", terrainMaterial });

	auto grassMaterial = make_shared<Material>();
	grassMaterial->SetMaterial(XMFLOAT3{ 0.01f, 0.01f, 0.01f }, 0.9f, XMFLOAT3{ 0.3f, 0.3f, 0.3f });
	grassMaterial->CreateShaderVariable(device);
	m_materials.insert({ "GRASS", grassMaterial });
}

inline void Scene::BuildObjects(const ComPtr<ID3D12Device>& device)
{
	m_lightSystem = make_unique<LightSystem>(device);
	auto sunLight = make_shared<DirectionalLight>();
	m_sun = make_unique<Sun>(sunLight);
	m_sun->SetStrength(XMFLOAT3{ 1.3f, 1.2f, 1.2f });
	m_lightSystem->SetDirectionalLight(sunLight);

	m_player = make_shared<Player>();
	m_player->SetPosition(XMFLOAT3{ 0.f, 0.f, 0.f });
	m_player->SetTextureIndex(0);

	for (int x = -10; x <= 10; x += 5) {
		for (int y = 0; y <= 20; y += 5) {
			for (int z = -10; z <= 10; z += 5) {
				auto light = make_shared<SpotLight>(
					XMFLOAT3{ 0.7f, 0.7f, 0.7f }, 
					XMFLOAT3{ 1.f, 0.f, 0.f },
					XMFLOAT3{ 0.f, 0.f, 0.f }, 
					1.f, 50.f, 80.f);
				m_lightSystem->SetSpotLight(light);
				auto object = make_shared<LightObject>(light);
				object->SetPosition(XMFLOAT3{
					static_cast<FLOAT>(x),
					static_cast<FLOAT>(y),
					static_cast<FLOAT>(z) });
				object->SetTextureIndex(1);
				m_objects.push_back(object);
			}
		}
	}
	m_instanceObject = make_unique<Instance>(device,
		static_pointer_cast<Mesh<TextureVertex>>(m_meshes["CUBE"]), static_cast<UINT>(m_objects.size() + 1));
	m_instanceObject->SetObjects(m_objects);
	m_instanceObject->SetObject(m_player);
	m_instanceObject->SetTexture(m_textures["CUBE"]);
	m_instanceObject->SetMaterial(m_materials["CUBE"]);

	m_camera = make_shared<ThirdPersonCamera>(device);
	m_camera->SetLens(0.25 * XM_PI, g_framework->GetAspectRatio(), 0.1f, 1000.f);
	m_player->SetCamera(m_camera);

	m_skybox = make_shared<GameObject>(device);
	m_skybox->SetMesh(m_meshes["SKYBOX"]);
	m_skybox->SetTexture(m_textures["SKYBOX"]);

	m_terrain = make_shared<Terrain>(device);
	m_terrain->SetMesh(m_meshes["TERRAIN"]);
	m_terrain->SetTexture(m_textures["TERRAIN"]);
	m_terrain->SetMaterial(m_materials["TERRAIN"]);
	m_terrain->SetPosition(XMFLOAT3{ 0.f, -30.f, 0.f });

	vector<shared_ptr<InstanceObject>> grasses;
	for (int x = -127; x <= 127; x += 1) {
		for (int z = -127; z <= 127; z += 1) {
			FLOAT fx = static_cast<FLOAT>(x);
			FLOAT fz = static_cast<FLOAT>(z);
			auto grass0 = make_shared<InstanceObject>();
			grass0->SetPosition(XMFLOAT3{ fx, m_terrain->GetHeight(fx, fz), fz });
			grass0->SetTextureIndex(grasses.size() % 4);
			grasses.push_back(grass0);
			//auto grass1 = make_shared<InstanceObject>();
			//grass1->SetPosition(XMFLOAT3{ fx + 0.5f, m_terrain->GetHeight(fx + 0.5f, fz), fz });
			//grass1->SetTextureIndex(grasses.size() % 4);
			//grasses.push_back(grass1);
			//auto grass2 = make_shared<InstanceObject>();
			//grass2->SetPosition(XMFLOAT3{ fx, m_terrain->GetHeight(fx, fz + 0.5f), fz + 0.5f });
			//grass2->SetTextureIndex(grasses.size() % 4);
			//grasses.push_back(grass2);
			//auto grass3 = make_shared<InstanceObject>();
			//grass3->SetPosition(XMFLOAT3{ fx + 0.5f, m_terrain->GetHeight(fx + 0.5f, fz + 0.5f), fz + 0.5f });
			//grass3->SetTextureIndex(grasses.size() % 4);
			//grasses.push_back(grass3);
		}
	}
	m_instanceBillboard = make_unique<Instance>(device,
		static_pointer_cast<Mesh<TextureVertex>>(m_meshes["BILLBOARD"]), static_cast<UINT>(grasses.size()));
	m_instanceBillboard->SetObjects(move(grasses));
	m_instanceBillboard->SetTexture(m_textures["GRASS"]);
	m_instanceBillboard->SetMaterial(m_materials["GRASS"]);
}

void Scene::ReleaseUploadBuffer()
{
	for (auto& mesh : views::values(m_meshes)) {
		mesh->ReleaseUploadBuffer();
	}
	for (auto& texture : views::values(m_textures)) {
		texture->ReleaseUploadBuffer();
	}
}

void Scene::MouseEvent(UINT message, LPARAM lParam)
{
}

void Scene::KeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}