#pragma once
#include "stdafx.h"

class Texture
{
public:
	Texture() = delete;
	Texture(const ComPtr<ID3D12Device>& device);
	Texture(const ComPtr<ID3D12Device>& device, 
		const ComPtr<ID3D12GraphicsCommandList>& commandList, 
		const wstring& fileName, UINT rootParameterIndex, BOOL createResourceView = true);
	~Texture() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void ReleaseUploadBuffer();

	void LoadTexture(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName, UINT rootParameterIndex);
	void CreateShaderVariable(const ComPtr<ID3D12Device>& device);

private:
	void CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	void CreateShaderResourceView(const ComPtr<ID3D12Device>& device);


private:
	UINT m_srvDescriptorSize;

	ComPtr<ID3D12DescriptorHeap>				m_srvDescriptorHeap;
	vector<pair<ComPtr<ID3D12Resource>, UINT>>	m_textures;
	vector<ComPtr<ID3D12Resource>>				m_textureUploadBuffer;
};

