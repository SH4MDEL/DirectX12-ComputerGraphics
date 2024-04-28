#include "mesh.h"

void MeshBase::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, size_t count) const
{
	commandList->IASetPrimitiveTopology(m_primitiveTopology);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->DrawInstanced(m_vertices, static_cast<UINT>(count), 0, 0);
}

void MeshBase::ReleaseUploadBuffer()
{
	if (m_vertexUploadBuffer) m_vertexUploadBuffer.Reset();
}

TerrainMesh::TerrainMesh(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName,
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_primitiveTopology = primitiveTopology;
	LoadMesh(device, commandList, fileName);
}

FLOAT TerrainMesh::GetHeight(FLOAT x, FLOAT z) const
{
	const XMFLOAT2 range = XMFLOAT2{
		static_cast<FLOAT>(-m_length / 2),
		static_cast<FLOAT>(+m_length / 2) };
	if (range.x > x || range.y < x || range.x > z || range.y < z) return 0.f;
	const size_t nx = static_cast<size_t>(x + m_length / 2);
	const size_t nz = static_cast<size_t>(z + m_length / 2);

	const XMFLOAT2 percentage = XMFLOAT2{ x - floor(x), z - floor(z) };
	if (percentage.x >= percentage.y) {
		return lerp(lerp(m_height[nz][nx], m_height[nz][nx + 1], percentage.x),
			m_height[nz + 1][nx + 1], percentage.y);
	}
	return lerp(lerp(m_height[nz][nx], m_height[nz + 1][nx], percentage.y),
		m_height[nz + 1][nx + 1], percentage.x);
}

void TerrainMesh::LoadMesh(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	ifstream in(fileName, ios::binary);
	in.seekg(0, ios::end);
	size_t size = static_cast<size_t>(in.tellg());
	m_length = static_cast<int>(sqrt(size));
	INT half = m_length / 2;

	m_height.resize(m_length, vector<FLOAT>(m_length));
	vector<BYTE> height(m_length);

	in.seekg(0, ios::beg);
	for (auto& line : m_height) {
		in.read(reinterpret_cast<char*>(height.data()), m_length * sizeof(BYTE));
		for (size_t i = 0; auto& dot : line) {
			dot = static_cast<FLOAT>(height[i++]);
			dot /= 3.f;
		}
	}

	m_grid = 1.f / static_cast<FLOAT>(m_length);
	vector<DetailVertex> vertices;
	for (int z = 0; z < m_length - 1; ++z) {
		for (int x = 0; x < m_length - 1; ++x) {
			FLOAT nx = static_cast<FLOAT>(x - half);
			FLOAT nz = static_cast<FLOAT>(z - half);
			FLOAT dx = static_cast<FLOAT>(x) * m_grid;
			FLOAT dz = 1.f - static_cast<FLOAT>(z) * m_grid;

			vertices.emplace_back(XMFLOAT3{ nx, m_height[z][x], nz },
				XMFLOAT2{ dx, dz }, XMFLOAT2{ 0.f, 1.f });
			vertices.emplace_back(XMFLOAT3{ nx, m_height[z + 1][x], nz + 1 },
				XMFLOAT2{ dx, dz - m_grid }, XMFLOAT2{ 0.f, 0.f });
			vertices.emplace_back(XMFLOAT3{ nx + 1, m_height[z + 1][x + 1], nz + 1 },
				XMFLOAT2{ dx + m_grid, dz - m_grid }, XMFLOAT2{ 1.f, 0.f });

			vertices.emplace_back(XMFLOAT3{ nx, m_height[z][x], nz },
				XMFLOAT2{ dx, dz }, XMFLOAT2{ 0.f, 1.f });
			vertices.emplace_back(XMFLOAT3{ nx + 1, m_height[z + 1][x + 1], nz + 1 },
				XMFLOAT2{ dx + m_grid, dz - m_grid }, XMFLOAT2{ 1.f, 0.f });
			vertices.emplace_back(XMFLOAT3{ nx + 1, m_height[z][x + 1], nz },
				XMFLOAT2{ dx + m_grid, dz }, XMFLOAT2{ 1.f, 1.f });
		}
	}

	CreateVertexBuffer(device, commandList, vertices);
}
