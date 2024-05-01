#include "shadow.h"

ShadowMap::ShadowMap(const ComPtr<ID3D12Device>& device, UINT width, UINT height) :
	Texture(device), m_width{width}, m_height{height},
	m_viewport{0.f, 0.f, static_cast<FLOAT>(width), static_cast<FLOAT>(height), 0.f, 1.f},
	m_scissorRect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) }
{
	m_rootParameterIndex = RootParameter::TextureShadow;
	CreateTexture(device);
	CreateShaderVariable(device);
}

void ShadowMap::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	ID3D12DescriptorHeap* ppHeaps[] = { m_srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	commandList->SetGraphicsRootDescriptorTable(RootParameter::TextureShadow,
		m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->RSSetViewports(1, &m_viewport);
	commandList->RSSetScissorRects(1, &m_scissorRect);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::
		Transition(m_textures[0].Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, 
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

	commandList->ClearDepthStencilView(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(0, nullptr, false, 
		&m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void ShadowMap::Close(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::
		Transition(m_textures[0].Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_GENERIC_READ));
}

void ShadowMap::CreateTexture(const ComPtr<ID3D12Device>& device)
{
	ComPtr<ID3D12Resource> texture;
	Utiles::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R24G8_TYPELESS,
			m_width, m_height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&CD3DX12_CLEAR_VALUE{ DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0 },
		IID_PPV_ARGS(&texture)));

	m_textures.push_back(texture);
}

void ShadowMap::CreateShaderVariable(const ComPtr<ID3D12Device>& device)
{
	CreateSrvDescriptorHeap(device);
	CreateDsvDescriptorHeap(device);
	CreateShaderResourceView(device);
	CreateDepthStencilView(device);
}

void ShadowMap::CreateDsvDescriptorHeap(const ComPtr<ID3D12Device>& device)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	Utiles::ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, 
		IID_PPV_ARGS(&m_dsvDescriptorHeap)));
}

void ShadowMap::CreateDepthStencilView(const ComPtr<ID3D12Device>& device)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle{
	m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(m_textures[0].Get(), &dsvDesc, descriptorHandle);
}
