#include "texture.h"
#include "../Common/DDSTextureLoader12.h"

Texture::Texture(const ComPtr<ID3D12Device>& device)
{
	m_srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

Texture::Texture(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList, 
	const wstring& fileName, UINT rootParameterIndex, BOOL createResourceView)
{
	m_srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	LoadTexture(device, commandList, fileName, rootParameterIndex);
	if (createResourceView) CreateShaderVariable(device);
}

void Texture::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	ID3D12DescriptorHeap* ppHeaps[] = { m_srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(m_rootParameterIndex,
		m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void Texture::ReleaseUploadBuffer()
{
	for (auto& uploadBuffer : m_textureUploadBuffer) {
		uploadBuffer.Reset();
	}
	m_textureUploadBuffer.clear();
}

void Texture::LoadTexture(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName, UINT rootParameterIndex)
{
	m_rootParameterIndex = rootParameterIndex;

	ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> textureUploadBuffer;

	unique_ptr<uint8_t[]> ddsData;
	vector<D3D12_SUBRESOURCE_DATA> subresources;
	DDS_ALPHA_MODE ddsAlphaMode{ DDS_ALPHA_MODE_UNKNOWN };
	Utiles::ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(device.Get(), fileName.c_str(), 0,
		D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, texture.GetAddressOf(), ddsData, subresources, &ddsAlphaMode));

	UINT nSubresources{ (UINT)subresources.size() };
	const UINT64 TextureSize{ GetRequiredIntermediateSize(texture.Get(), 0, nSubresources) };

	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(TextureSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureUploadBuffer)
	));

	UpdateSubresources(commandList.Get(), texture.Get(), textureUploadBuffer.Get(), 0, 0, nSubresources, subresources.data());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	m_textures.push_back(texture);
	m_textureUploadBuffer.push_back(textureUploadBuffer);
}

void Texture::CreateShaderVariable(const ComPtr<ID3D12Device>& device)
{
	CreateSrvDescriptorHeap(device);
	CreateShaderResourceView(device);
}

void Texture::CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = static_cast<UINT>(m_textures.size());
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	Utiles::ThrowIfFailed(device->CreateDescriptorHeap(
		&srvHeapDesc, IID_PPV_ARGS(&m_srvDescriptorHeap)));
}

void Texture::CreateShaderResourceView(const ComPtr<ID3D12Device>& device)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle{ 
		m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	for (const auto& texture : m_textures) {
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (m_rootParameterIndex)
		{
		case RootParameter::Texture:
			srvDesc.Format = texture->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = texture->GetDesc().MipLevels;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
			break;
		case RootParameter::TextureCube:
			srvDesc.Format = texture->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = texture->GetDesc().MipLevels;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.f;
			break;
		case RootParameter::TextureShadow:
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = texture->GetDesc().MipLevels;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
			srvDesc.Texture2D.PlaneSlice = 0;
			break;
		default:
			break;
		}
		device->CreateShaderResourceView(texture.Get(), &srvDesc, descriptorHandle);

		descriptorHandle.Offset(1, m_srvDescriptorSize);
	}
}
