#pragma once
#include "stdafx.h"

// Index Buffer 미사용
class Mesh abstract
{
public:
	Mesh() = default;
	virtual ~Mesh() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void ReleaseUploadBuffer();

	struct VertexBase {};

protected:
	template <typename T> requires derived_from<T, Mesh::VertexBase>
	void LoadMesh(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);

protected:
	UINT						m_vertices;
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	ComPtr<ID3D12Resource>		m_vertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView;
};

// Index Buffer 사용
class IndexMesh abstract : public Mesh
{
public:
	IndexMesh() = default;
	~IndexMesh() override = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;
	virtual void ReleaseUploadBuffer() override;

protected:
	template <typename T> requires derived_from<T, Mesh::VertexBase>
	void LoadIndexMesh(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);

protected:
	UINT						m_indices;
	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_indexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
};

class CubeMesh : public Mesh
{
private:
	struct Vertex : public VertexBase
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

public:
	CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName);
	~CubeMesh() override = default;
};

class CubeIndexMesh : public IndexMesh
{
private:
	struct Vertex : public VertexBase
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	CubeIndexMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName);
	~CubeIndexMesh() override = default;
};

class SkyboxMesh : public Mesh
{
private:
	struct Vertex : public VertexBase
	{
		XMFLOAT3 position;
	};

public:
	SkyboxMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName);
	~SkyboxMesh() override = default;
};

class TerrainMesh : public Mesh
{
private:
	struct Vertex : public VertexBase
	{
		Vertex(XMFLOAT3 p, XMFLOAT2 u0, XMFLOAT2 u1) : position{ p }, uv0{ u0 }, uv1{ u1 } {};
		XMFLOAT3 position;
		XMFLOAT2 uv0;
		XMFLOAT2 uv1;
	};

public:
	TerrainMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName);
	~TerrainMesh() override = default;

	FLOAT GetHeight(INT x, INT z) const;

private:
	vector<vector<BYTE>> m_height;
};

template <typename T> requires derived_from<T, Mesh::VertexBase>
inline void Mesh::LoadMesh(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	ifstream in(fileName, ios::binary);

	UINT vertexNum;
	in >> vertexNum;

	vector<T> vertices;
	vertices.resize(vertexNum);
	in.read(reinterpret_cast<char*>(vertices.data()), vertexNum * sizeof(T));

	m_vertices = static_cast<UINT>(vertices.size());
	const UINT vertexBufferSize = m_vertices * sizeof(T);

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
	m_vertexBufferView.StrideInBytes = sizeof(T);
}

template<typename T> requires derived_from<T, Mesh::VertexBase>
inline void IndexMesh::LoadIndexMesh(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	ifstream in(fileName, ios::binary);

	UINT vertexNum;
	in >> vertexNum;

	vector<T> vertices;
	vertices.resize(vertexNum);
	in.read(reinterpret_cast<char*>(vertices.data()), vertexNum * sizeof(T));

	UINT indiceNum;
	in >> indiceNum;

	vector<UINT> indices;
	indices.resize(indiceNum);
	in.read(reinterpret_cast<char*>(indices.data()), indiceNum * sizeof(UINT));

	m_vertices = static_cast<UINT>(vertices.size());
	const UINT vertexBufferSize = m_vertices * sizeof(T);

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
	m_vertexBufferView.StrideInBytes = sizeof(T);

	m_indices = static_cast<UINT>(indices.size());
	const UINT indexBufferSize = m_indices * sizeof(UINT);

	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		NULL,
		IID_PPV_ARGS(&m_indexBuffer)));

	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_indexUploadBuffer)));

	D3D12_SUBRESOURCE_DATA indexData{};
	indexData.pData = indices.data();
	indexData.RowPitch = indexBufferSize;
	indexData.SlicePitch = indexData.RowPitch;
	UpdateSubresources<1>(commandList.Get(), m_indexBuffer.Get(), m_indexUploadBuffer.Get(), 0, 0, 1, &indexData);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = indexBufferSize;
}
