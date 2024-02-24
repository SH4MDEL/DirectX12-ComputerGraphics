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

CubeMesh::CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	vector<Vertex> vertices;
	const XMFLOAT3 LEFTDOWNFRONT =	{ -1.f, -1.f, -1.f };
	const XMFLOAT3 LEFTDOWNBACK =	{ -1.f, -1.f, +1.f };
	const XMFLOAT3 LEFTUPFRONT =	{ -1.f, +1.f, -1.f };
	const XMFLOAT3 LEFTUPBACK =		{ -1.f, +1.f, +1.f };
	const XMFLOAT3 RIGHTDOWNFRONT = { +1.f, -1.f, -1.f };
	const XMFLOAT3 RIGHTDOWNBACK =	{ +1.f, -1.f, +1.f };
	const XMFLOAT3 RIGHTUPFRONT =	{ +1.f, +1.f, -1.f };
	const XMFLOAT3 RIGHTUPBACK =	{ +1.f, +1.f, +1.f };

	// Front
	vertices.emplace_back(LEFTUPFRONT,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT,		XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNFRONT,	XMFLOAT2{ 1.0f, 1.0f });
	
	vertices.emplace_back(LEFTUPFRONT,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNFRONT,	XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNFRONT,	XMFLOAT2{ 0.0f, 1.0f });
	
	// Up
	vertices.emplace_back(LEFTUPBACK,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPBACK,		XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT,		XMFLOAT2{ 1.0f, 1.0f });
	
	vertices.emplace_back(LEFTUPBACK,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPFRONT,		XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTUPFRONT,		XMFLOAT2{ 0.0f, 1.0f });
	
	// Back
	vertices.emplace_back(LEFTDOWNBACK,		XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK,	XMFLOAT2{ 0.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK,		XMFLOAT2{ 0.0f, 0.0f });
	
	vertices.emplace_back(LEFTDOWNBACK,		XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTUPBACK,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTUPBACK,		XMFLOAT2{ 1.0f, 0.0f });
	
	// Down
	vertices.emplace_back(LEFTDOWNFRONT,	XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT,	XMFLOAT2{ 0.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK,	XMFLOAT2{ 0.0f, 0.0f });
	
	vertices.emplace_back(LEFTDOWNFRONT,	XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNBACK,	XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNBACK,		XMFLOAT2{ 1.0f, 0.0f });
	
	// Left
	vertices.emplace_back(LEFTUPBACK,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTUPFRONT,		XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNFRONT,	XMFLOAT2{ 1.0f, 1.0f });
																 
	vertices.emplace_back(LEFTUPBACK,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(LEFTDOWNFRONT,	XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(LEFTDOWNBACK,		XMFLOAT2{ 0.0f, 1.0f });
																 
	// Right													 
	vertices.emplace_back(RIGHTUPFRONT,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTUPBACK,		XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNBACK,	XMFLOAT2{ 1.0f, 1.0f });
																 
	vertices.emplace_back(RIGHTUPFRONT,		XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(RIGHTDOWNBACK,	XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(RIGHTDOWNFRONT,	XMFLOAT2{ 0.0f, 1.0f });

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

CubeIndexMesh::CubeIndexMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
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

	vector<UINT> indices;
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
	UpdateSubresources<1>(commandList.Get(), m_vertexBuffer.Get(), m_vertexUploadBuffer.Get(), 0, 0, 1, &vertexData);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = vertexBufferSize;
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);

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

SkyboxMesh::SkyboxMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
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
