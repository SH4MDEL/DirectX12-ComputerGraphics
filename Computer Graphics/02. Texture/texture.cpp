#include "texture.h"
#include "../Common/DDSTextureLoader12.h"

Texture::Texture(const ComPtr<ID3D12Device>& device, 
	const ComPtr<ID3D12GraphicsCommandList>& commandList, 
	const wstring& fileName, UINT rootParameterIndex) : 
	m_rootParameterIndex{rootParameterIndex}
{
	LoadTexture(device, commandList, fileName);
	CreateSrvDescriptorHeap(device);
	CreateShaderResourceView(device);
}

void Texture::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	ID3D12DescriptorHeap* ppHeaps[] = { m_srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle{ m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	commandList->SetGraphicsRootDescriptorTable(m_rootParameterIndex, descriptorHandle);
}

void Texture::ReleaseUploadBuffer()
{
	m_textureUploadBuffer.Reset();
}

void Texture::LoadTexture(const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName)
{
	unique_ptr<uint8_t[]> ddsData;
	vector<D3D12_SUBRESOURCE_DATA> subresources;
	DDS_ALPHA_MODE ddsAlphaMode{ DDS_ALPHA_MODE_UNKNOWN };
	Utiles::ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(device.Get(), fileName.c_str(), 0,
		D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, m_texture.GetAddressOf(), ddsData, subresources, &ddsAlphaMode));

	UINT nSubresources{ (UINT)subresources.size() };
	const UINT64 TextureSize{ GetRequiredIntermediateSize(m_texture.Get(), 0, nSubresources) };

	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(TextureSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_textureUploadBuffer)
	));

	UpdateSubresources(commandList.Get(), m_texture.Get(), m_textureUploadBuffer.Get(), 0, 0, nSubresources, subresources.data());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void Texture::CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	Utiles::ThrowIfFailed(device->CreateDescriptorHeap(
		&srvHeapDesc, IID_PPV_ARGS(&m_srvDescriptorHeap)));
}

void Texture::CreateShaderResourceView(const ComPtr<ID3D12Device>& device)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle{ 
		m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_texture->GetDesc().Format;

	switch (m_rootParameterIndex)
	{
	case RootParameter::Texture:
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_texture->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RootParameter::TextureCube:
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = m_texture->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	default:
		break;
	}
	device->CreateShaderResourceView(m_texture.Get(), &srvDesc, descriptorHandle);
}
