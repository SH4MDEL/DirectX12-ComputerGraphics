#pragma once
#include "stdafx.h"
#include "vertex.h"

class MeshBase abstract
{
public:
	MeshBase() = default;
	virtual ~MeshBase() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, size_t count = 1) const;
	virtual void ReleaseUploadBuffer();

protected:
	UINT						m_vertices;
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	ComPtr<ID3D12Resource>		m_vertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY	m_primitiveTopology;
};

template <typename T> requires derived_from<T, VertexBase>
class Mesh : public MeshBase
{
public:
	Mesh() = default;
	Mesh(const ComPtr<ID3D12Device>& device, 
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName, 
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~Mesh() override = default;

protected:
	virtual void LoadMesh(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);

	void CreateVertexBuffer(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<T>& vertices);
};

template<typename T> requires derived_from<T, VertexBase>
inline Mesh<T>::Mesh(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName,
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_primitiveTopology = primitiveTopology;
	LoadMesh(device, commandList, fileName);
}

template<typename T> requires derived_from<T, VertexBase>
inline void Mesh<T>::LoadMesh(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	ifstream in(fileName, ios::binary);

	UINT vertexNum;
	in >> vertexNum;

	vector<T> vertices;
	vertices.resize(vertexNum);
	in.read(reinterpret_cast<char*>(vertices.data()), vertexNum * sizeof(T));

	CreateVertexBuffer(device, commandList, vertices);
}

template<typename T> requires derived_from<T, VertexBase>
inline void Mesh<T>::CreateVertexBuffer(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<T>& vertices)
{
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

template <typename T> requires derived_from<T, VertexBase>
class IndexMesh : public Mesh<T>
{
public:
	IndexMesh() = default;
	IndexMesh(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName,
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~IndexMesh() override = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;
	virtual void ReleaseUploadBuffer() override;

protected:
	virtual void LoadMesh(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName) override;

	void CreateIndexBuffer(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<UINT>& indices);

protected:
	UINT						m_indices;
	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_indexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
};

template<typename T> requires derived_from<T, VertexBase>
inline IndexMesh<T>::IndexMesh(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName,
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_primitiveTopology = primitiveTopology;
	LoadMesh(device, commandList, fileName);
}

template<typename T> requires derived_from<T, VertexBase>
inline void IndexMesh<T>::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->IASetPrimitiveTopology(m_primitiveTopology);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetIndexBuffer(&m_indexBufferView);
	commandList->DrawIndexedInstanced(m_indices, 1, 0, 0, 0);
}

template<typename T> requires derived_from<T, VertexBase>
inline void IndexMesh<T>::ReleaseUploadBuffer()
{
	MeshBase::ReleaseUploadBuffer();
	if (m_indexUploadBuffer) m_indexUploadBuffer.Reset();
}

template<typename T> requires derived_from<T, VertexBase>
inline void IndexMesh<T>::LoadMesh(const ComPtr<ID3D12Device>& device, 
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

	CreateVertexBuffer(device, commandList, vertices);
	CreateIndexBuffer(device, commandList, indices);
}

template<typename T> requires derived_from<T, VertexBase>
inline void IndexMesh<T>::CreateIndexBuffer(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<UINT>& indices)
{
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

class TerrainMesh : public Mesh<TerrainVertex>
{
public:
	TerrainMesh(const ComPtr<ID3D12Device>& device, 
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);
	~TerrainMesh() override = default;

	FLOAT GetHeight(FLOAT x, FLOAT z);

private:
	void LoadMesh(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName) override;

	void CreatePatch(vector<TerrainVertex>& vertices, INT zStart, INT zEnd, INT xStart, INT xEnd);

	void BernsteinBasis(FLOAT t, FLOAT* basis);
	FLOAT GetBezierSumHeight(INT sx, INT sz, FLOAT* basisU, FLOAT* basisV);

private:
	vector<vector<FLOAT>> m_height;
	INT m_length;
	INT m_patchLength;
};
