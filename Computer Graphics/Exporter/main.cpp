#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

void CreateCubeMesh()
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	vector<Vertex> vertices;
	const XMFLOAT3 LEFTDOWNFRONT = { -1.f, -1.f, -1.f };
	const XMFLOAT3 LEFTDOWNBACK = { -1.f, -1.f, +1.f };
	const XMFLOAT3 LEFTUPFRONT = { -1.f, +1.f, -1.f };
	const XMFLOAT3 LEFTUPBACK = { -1.f, +1.f, +1.f };
	const XMFLOAT3 RIGHTDOWNFRONT = { +1.f, -1.f, -1.f };
	const XMFLOAT3 RIGHTDOWNBACK = { +1.f, -1.f, +1.f };
	const XMFLOAT3 RIGHTUPFRONT = { +1.f, +1.f, -1.f };
	const XMFLOAT3 RIGHTUPBACK = { +1.f, +1.f, +1.f };

	// Front
	vertices.emplace_back(LEFTUPFRONT, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(LEFTUPFRONT, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNFRONT, XMFLOAT2{ 0.0f, 1.0f });

	// Up
	vertices.emplace_back(LEFTUPBACK, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPBACK, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(LEFTUPBACK, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTUPFRONT, XMFLOAT2{ 0.0f, 1.0f });

	// Back
	vertices.emplace_back(LEFTDOWNBACK, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, XMFLOAT2{ 0.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK, XMFLOAT2{ 0.0f, 0.0f });

	vertices.emplace_back(LEFTDOWNBACK, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTUPBACK, XMFLOAT2{ 1.0f, 0.0f });

	// Down
	vertices.emplace_back(LEFTDOWNFRONT, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, XMFLOAT2{ 0.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, XMFLOAT2{ 0.0f, 0.0f });

	vertices.emplace_back(LEFTDOWNFRONT, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNBACK, XMFLOAT2{ 1.0f, 0.0f });

	// Left
	vertices.emplace_back(LEFTUPBACK, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTUPFRONT, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNFRONT, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(LEFTUPBACK, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNFRONT, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNBACK, XMFLOAT2{ 0.0f, 1.0f });

	// Right													 
	vertices.emplace_back(RIGHTUPFRONT, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPBACK, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNBACK, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(RIGHTUPFRONT, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNBACK, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, XMFLOAT2{ 0.0f, 1.0f });

	ofstream out("../Resources/Meshes/CubeMesh.binary", ios::binary);
	out << vertices.size();
	out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(Vertex)* vertices.size());
}

void CreateSkyboxMesh()
{
	struct Vertex
	{
		XMFLOAT3 position;
	};

	vector<Vertex> vertices;
	const XMFLOAT3 LEFTDOWNFRONT = { -1.f, -1.f, -1.f };
	const XMFLOAT3 LEFTDOWNBACK = { -1.f, -1.f, +1.f };
	const XMFLOAT3 LEFTUPFRONT = { -1.f, +1.f, -1.f };
	const XMFLOAT3 LEFTUPBACK = { -1.f, +1.f, +1.f };
	const XMFLOAT3 RIGHTDOWNFRONT = { +1.f, -1.f, -1.f };
	const XMFLOAT3 RIGHTDOWNBACK = { +1.f, -1.f, +1.f };
	const XMFLOAT3 RIGHTUPFRONT = { +1.f, +1.f, -1.f };
	const XMFLOAT3 RIGHTUPBACK = { +1.f, +1.f, +1.f };

	// Front
	vertices.emplace_back(LEFTUPFRONT);
	vertices.emplace_back(RIGHTUPFRONT);
	vertices.emplace_back(RIGHTDOWNFRONT);

	vertices.emplace_back(LEFTUPFRONT);
	vertices.emplace_back(RIGHTDOWNFRONT);
	vertices.emplace_back(LEFTDOWNFRONT);

	// Up
	vertices.emplace_back(LEFTUPBACK);
	vertices.emplace_back(RIGHTUPBACK);
	vertices.emplace_back(RIGHTUPFRONT);

	vertices.emplace_back(LEFTUPBACK);
	vertices.emplace_back(RIGHTUPFRONT);
	vertices.emplace_back(LEFTUPFRONT);

	// Back
	vertices.emplace_back(LEFTDOWNBACK);
	vertices.emplace_back(RIGHTDOWNBACK);
	vertices.emplace_back(RIGHTUPBACK);

	vertices.emplace_back(LEFTDOWNBACK);
	vertices.emplace_back(RIGHTUPBACK);
	vertices.emplace_back(LEFTUPBACK);

	// Down
	vertices.emplace_back(LEFTDOWNFRONT);
	vertices.emplace_back(RIGHTDOWNFRONT);
	vertices.emplace_back(RIGHTDOWNBACK);

	vertices.emplace_back(LEFTDOWNFRONT);
	vertices.emplace_back(RIGHTDOWNBACK);
	vertices.emplace_back(LEFTDOWNBACK);

	// Left
	vertices.emplace_back(LEFTUPBACK);
	vertices.emplace_back(LEFTUPFRONT);
	vertices.emplace_back(LEFTDOWNFRONT);

	vertices.emplace_back(LEFTUPBACK);
	vertices.emplace_back(LEFTDOWNFRONT);
	vertices.emplace_back(LEFTDOWNBACK);

	// Right													 
	vertices.emplace_back(RIGHTUPFRONT);
	vertices.emplace_back(RIGHTUPBACK);
	vertices.emplace_back(RIGHTDOWNBACK);

	vertices.emplace_back(RIGHTUPFRONT);
	vertices.emplace_back(RIGHTDOWNBACK);
	vertices.emplace_back(RIGHTDOWNFRONT);

	ofstream out("../Resources/Meshes/SkyboxMesh.binary", ios::binary);
	out << vertices.size();
	out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(Vertex)* vertices.size());
}

void CreateCubeIndexMesh()
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	vector<Vertex> vertices;

	const XMFLOAT3 LEFTDOWNFRONT = { -1.f, -1.f, -1.f };
	const XMFLOAT3 LEFTDOWNBACK = { -1.f, -1.f, +1.f };
	const XMFLOAT3 LEFTUPFRONT = { -1.f, +1.f, -1.f };
	const XMFLOAT3 LEFTUPBACK = { -1.f, +1.f, +1.f };
	const XMFLOAT3 RIGHTDOWNFRONT = { +1.f, -1.f, -1.f };
	const XMFLOAT3 RIGHTDOWNBACK = { +1.f, -1.f, +1.f };
	const XMFLOAT3 RIGHTUPFRONT = { +1.f, +1.f, -1.f };
	const XMFLOAT3 RIGHTUPBACK = { +1.f, +1.f, +1.f };

	vertices.emplace_back(LEFTUPBACK, XMFLOAT4{ 1.0f, 1.0f, 0.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK, XMFLOAT4{ 0.0f, 0.5f, 0.5f, 1.0f });
	vertices.emplace_back(RIGHTUPFRONT, XMFLOAT4{ 0.5f, 0.5f, 0.0f, 1.0f });
	vertices.emplace_back(LEFTUPFRONT, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f });

	vertices.emplace_back(LEFTDOWNBACK, XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, XMFLOAT4{ 0.0f, 1.0f, 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, XMFLOAT4{ 1.0f, 0.0f, 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNFRONT, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });

	vector<unsigned> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);

	indices.push_back(3); indices.push_back(2); indices.push_back(6);
	indices.push_back(3); indices.push_back(6); indices.push_back(7);

	indices.push_back(7); indices.push_back(6); indices.push_back(5);
	indices.push_back(7); indices.push_back(5); indices.push_back(4);

	indices.push_back(1); indices.push_back(0); indices.push_back(4);
	indices.push_back(1); indices.push_back(4); indices.push_back(5);

	indices.push_back(0); indices.push_back(3); indices.push_back(7);
	indices.push_back(0); indices.push_back(7); indices.push_back(4);

	indices.push_back(2); indices.push_back(1); indices.push_back(5);
	indices.push_back(2); indices.push_back(5); indices.push_back(6);

	ofstream out("../Resources/Meshes/CubeIndexMesh.binary", ios::binary);
	out << vertices.size();
	out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(Vertex) * vertices.size());
	out << indices.size();
	out.write(reinterpret_cast<const char*>(indices.data()), sizeof(unsigned) * indices.size());
}

void CreateBillboardMesh()
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 size;
	};

	vector<Vertex> vertices(1, {XMFLOAT3{0.f, 0.f, 0.f}, XMFLOAT2{1.f, 1.f}});

	ofstream out("../Resources/Meshes/BillboardMesh.binary", ios::binary);
	out << vertices.size();
	out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(Vertex) * vertices.size());
}

void CreateCubeNormalMesh()
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

	vector<Vertex> vertices;
	const XMFLOAT3 LEFTDOWNFRONT = { -1.f, -1.f, -1.f };
	const XMFLOAT3 LEFTDOWNBACK = { -1.f, -1.f, +1.f };
	const XMFLOAT3 LEFTUPFRONT = { -1.f, +1.f, -1.f };
	const XMFLOAT3 LEFTUPBACK = { -1.f, +1.f, +1.f };
	const XMFLOAT3 RIGHTDOWNFRONT = { +1.f, -1.f, -1.f };
	const XMFLOAT3 RIGHTDOWNBACK = { +1.f, -1.f, +1.f };
	const XMFLOAT3 RIGHTUPFRONT = { +1.f, +1.f, -1.f };
	const XMFLOAT3 RIGHTUPBACK = { +1.f, +1.f, +1.f };

	XMFLOAT3 NORMAL = { 0.f, 0.f, -1.f };
	// Front
	vertices.emplace_back(LEFTUPFRONT, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT, NORMAL, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(LEFTUPFRONT, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNFRONT, NORMAL, XMFLOAT2{ 0.0f, 1.0f });

	NORMAL = { 0.f, 1.f, 0.f };
	// Up
	vertices.emplace_back(LEFTUPBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPBACK, NORMAL, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(LEFTUPBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTUPFRONT, NORMAL, XMFLOAT2{ 0.0f, 1.0f });

	NORMAL = { 0.f, 0.f, 1.f };
	// Back
	vertices.emplace_back(LEFTDOWNBACK, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, NORMAL, XMFLOAT2{ 0.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });

	vertices.emplace_back(LEFTDOWNBACK, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTUPBACK, NORMAL, XMFLOAT2{ 1.0f, 0.0f });

	NORMAL = { 0.f, -1.f, 0.f };
	// Down
	vertices.emplace_back(LEFTDOWNFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, NORMAL, XMFLOAT2{ 0.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });

	vertices.emplace_back(LEFTDOWNFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNBACK, NORMAL, XMFLOAT2{ 1.0f, 0.0f });

	NORMAL = { -1.f, 0.f, 0.f };
	// Left
	vertices.emplace_back(LEFTUPBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTUPFRONT, NORMAL, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(LEFTUPBACK, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNFRONT, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNBACK, NORMAL, XMFLOAT2{ 0.0f, 1.0f });

	NORMAL = { 1.f, 0.f, 0.f };
	// Right													 
	vertices.emplace_back(RIGHTUPFRONT, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPBACK, NORMAL, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNBACK, NORMAL, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(RIGHTUPFRONT, NORMAL, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNBACK, NORMAL, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT, NORMAL, XMFLOAT2{ 0.0f, 1.0f });

	ofstream out("../Resources/Meshes/CubeNormalMesh.binary", ios::binary);
	out << vertices.size();
	out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(Vertex) * vertices.size());
}

int main()
{
	//CreateCubeMesh();
	//CreateCubeIndexMesh();
	//CreateSkyboxMesh();
	//CreateBillboardMesh();
	CreateCubeNormalMesh();
}