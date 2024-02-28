#include "mesh.h"

void Mesh::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->DrawInstanced(m_vertices, 1, 0, 0);
}

void Mesh::ReleaseUploadBuffer()
{
	if (m_vertexUploadBuffer) m_vertexUploadBuffer.Reset();
}

void IndexMesh::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetIndexBuffer(&m_indexBufferView);
	commandList->DrawIndexedInstanced(m_indices, 1, 0, 0, 0);
}

void IndexMesh::ReleaseUploadBuffer()
{
	Mesh::ReleaseUploadBuffer();
	if (m_indexUploadBuffer) m_indexUploadBuffer.Reset();
}

CubeMesh::CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName)
{
	LoadMesh<Vertex>(device, commandList, fileName);
}

CubeIndexMesh::CubeIndexMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName)
{
	LoadIndexMesh<Vertex>(device, commandList, fileName);
}

SkyboxMesh::SkyboxMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName)
{
	LoadMesh<Vertex>(device, commandList, fileName);
}

TerrainMesh::TerrainMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, 
	const wstring& fileName)
{
	ifstream in(fileName, ios::binary);
	in.seekg(0, ios::end);
	size_t size = static_cast<size_t>(in.tellg());
	int length = static_cast<int>(sqrt(size));
	int middle = length / 2;

	m_height.resize(length, vector<BYTE>(length));
	
	in.seekg(0, ios::beg);
	for (auto& line : m_height) {
		in.read(reinterpret_cast<char*>(line.data()), length * sizeof(BYTE));
	}

	const float delta = 1.f / static_cast<float>(length);
	vector<Vertex> vertices;
	for (int z = 0; z < length - 1; ++z) {
		for (int x = 0; x < length - 1; ++x) {
			float nx = static_cast<float>(x - middle);
			float nz = static_cast<float>(z - middle);
			float dx = static_cast<float>(x) * delta;
			float dz = 1.f - static_cast<float>(z) * delta;

			vertices.emplace_back(XMFLOAT3{ nx, static_cast<float>(m_height[z][x]), nz },
				XMFLOAT2{dx, dz}, XMFLOAT2{0.f, 1.f});
			vertices.emplace_back(XMFLOAT3{ nx, static_cast<float>(m_height[z + 1][x]), nz + 1 },
				XMFLOAT2{dx, dz - delta}, XMFLOAT2{0.f, 0.f});
			vertices.emplace_back(XMFLOAT3{ nx + 1, static_cast<float>(m_height[z + 1][x + 1]), nz + 1 },
				XMFLOAT2{dx + delta, dz - delta}, XMFLOAT2{1.f, 0.f});

			vertices.emplace_back(XMFLOAT3{ nx, static_cast<float>(m_height[z][x]), nz },
				XMFLOAT2{dx, dz}, XMFLOAT2{0.f, 1.f});
			vertices.emplace_back(XMFLOAT3{ nx + 1, static_cast<float>(m_height[z + 1][x + 1]), nz + 1 },
				XMFLOAT2{dx + delta, dz - delta}, XMFLOAT2{1.f, 0.f});
			vertices.emplace_back(XMFLOAT3{ nx + 1, static_cast<float>(m_height[z][x + 1]), nz },
				XMFLOAT2{dx + delta, dz}, XMFLOAT2{1.f, 1.f});
		}
	}

	m_vertices = static_cast<UINT>(vertices.size());
	const UINT vertexBufferSize = m_vertices * sizeof(Vertex);

	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer)));

	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexUploadBuffer)));

	D3D12_SUBRESOURCE_DATA vertexData{};
	vertexData.pData = vertices.data();
	vertexData.RowPitch = vertexBufferSize;
	vertexData.SlicePitch = vertexData.RowPitch;
	UpdateSubresources<1>(commandList.Get(),
		m_vertexBuffer.Get(), m_vertexUploadBuffer.Get(), 0, 0, 1, &vertexData);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = vertexBufferSize;
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}

FLOAT TerrainMesh::GetHeight(INT x, INT z) const
{
	return static_cast<FLOAT>(m_height[x][z]);
}
