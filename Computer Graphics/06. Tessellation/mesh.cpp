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
	const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	LoadMesh(device, commandList, fileName);
}

FLOAT TerrainMesh::GetHeight(FLOAT x, FLOAT z)
{
	const XMFLOAT2 range = XMFLOAT2{
		static_cast<FLOAT>(-m_length / 2),
		static_cast<FLOAT>(+m_length / 2) };
	if (range.x > x || range.y < x || range.x > z || range.y < z) return 0.f;
	const INT nx = static_cast<INT>(x + m_length / 2);
	const INT nz = static_cast<INT>(z + m_length / 2);
	const INT sx = nx - nx % 4;
	const INT sz = nz - nz % 4;
	const FLOAT fx = x + m_length / 2;
	const FLOAT fz = z + m_length / 2;

	const XMFLOAT2 domainLocation{ (fx - sx) / 4.f, (fz - sz) / 4.f };

	FLOAT basisU[5], basisV[5];
	BernsteinBasis(domainLocation.x, basisU);
	BernsteinBasis(domainLocation.y, basisV);

	return GetBezierSumHeight(sx, sz, basisU, basisV);
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
		}
	}
	constexpr INT dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	constexpr INT dz[] = { 1, 1, 1, 0, 0, -1, -1, -1 };
	constexpr INT PatchSize{ 5 };
	constexpr INT BezierPoint{ PatchSize - 1 };
	m_grid = 1.f / static_cast<FLOAT>(m_length);
	vector<TerrainVertex> vertices;
	for (INT gz = (m_length - 1) / BezierPoint; gz > 0; --gz) {
		for (INT gx = 0; gx < (m_length - 1) / BezierPoint; ++gx) {
			XMINT2 Range{ gz * BezierPoint, gx * BezierPoint };
			for (INT z = Range.x; z >= Range.x - BezierPoint; --z) {
				for (INT x = Range.y; x <= Range.y + BezierPoint; ++x) {
					FLOAT nx = static_cast<FLOAT>(x - half);
					FLOAT nz = static_cast<FLOAT>(z - half);
					const XMFLOAT2 uv0{ static_cast<FLOAT>(x) / (m_length - 1),
						1.f - static_cast<FLOAT>(z) / (m_length - 1) };
					const XMFLOAT2 uv1{ static_cast<FLOAT>(x - Range.y) / static_cast<FLOAT>(BezierPoint),
						static_cast<FLOAT>(Range.x - z) / static_cast<FLOAT>(BezierPoint) };

					INT density = 0;
					for (int i = 0; i < 8; ++i) {
						INT tz = z + dz[i], tx = x + dx[i];
						if (tz < 0 || tz >= m_length || tx < 0 || tx >= m_length) continue;
						density = max(density, static_cast<INT>(abs(m_height[z][x] - m_height[tz][tx])));
					}

					vertices.emplace_back(
						XMFLOAT3{ nx, m_height[z][x], nz }, uv0, uv1, density);
				}
			}
		}
	}

	CreateVertexBuffer(device, commandList, vertices);
}

void TerrainMesh::BernsteinBasis(FLOAT t, FLOAT* basis)
{
	const FLOAT invT = 1.f - t;
	basis[0] = invT * invT * invT * invT;
	basis[1] = 4.f * t * invT * invT * invT;
	basis[2] = 6.f * t * t * invT * invT;
	basis[3] = 4.f * t * t * t * invT;
	basis[4] = t * t * t * t;
}

FLOAT TerrainMesh::GetBezierSumHeight(INT sx, INT sz, FLOAT* basisU, FLOAT* basisV)
{
	FLOAT sum = 0.f;
	sum += basisV[0] * (basisU[0] * m_height[sz][sx] + basisU[1] * m_height[sz][sx + 1] + basisU[2] * 
		m_height[sz][sx + 2] + basisU[3] * m_height[sz][sx + 3] + basisU[4] * m_height[sz][sx + 4]);
	sum += basisV[1] * (basisU[0] * m_height[sz + 1][sx] + basisU[1] * m_height[sz + 1][sx + 1] + basisU[2] * 
		m_height[sz + 1][sx + 2] + basisU[3] * m_height[sz + 1][sx + 3] + basisU[4] * m_height[sz + 1][sx + 4]);
	sum += basisV[2] * (basisU[0] * m_height[sz + 2][sx] + basisU[1] * m_height[sz + 2][sx + 1] + basisU[2] * 
		m_height[sz + 2][sx + 2] + basisU[3] * m_height[sz + 2][sx + 3] + basisU[4] * m_height[sz + 2][sx + 4]);
	sum += basisV[3] * (basisU[0] * m_height[sz + 3][sx] + basisU[1] * m_height[sz + 3][sx + 1] + basisU[2] * 
		m_height[sz + 3][sx + 2] + basisU[3] * m_height[sz + 3][sx + 3] + basisU[4] * m_height[sz + 3][sx + 4]);
	sum += basisV[4] * (basisU[0] * m_height[sz + 4][sx] + basisU[1] * m_height[sz + 4][sx + 1] + basisU[2] * 
		m_height[sz + 4][sx + 2] + basisU[3] * m_height[sz + 4][sx + 3] + basisU[4] * m_height[sz + 4][sx + 4]);
	return sum;
}
