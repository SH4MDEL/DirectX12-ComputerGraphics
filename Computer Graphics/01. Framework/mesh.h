#pragma once
#include "stdafx.h"

// Index Buffer 미사용
class Mesh abstract
{
public:
	Mesh() = default;
	~Mesh() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void ReleaseUploadBuffer();

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
	~IndexMesh() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;
	virtual void ReleaseUploadBuffer() override;

protected:
	UINT						m_indices;
	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_indexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
};

class CubeMesh : public Mesh
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 colors;
	};

public:
	CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList);
	~CubeMesh() = default;
};

class CubeIndexMesh : public IndexMesh
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 colors;
	};

public:
	CubeIndexMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList);
	~CubeIndexMesh() = default;
};