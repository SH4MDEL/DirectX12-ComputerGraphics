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

	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void ReleaseUploadBuffer();

	void LoadTexture(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName, UINT rootParameterIndex);
	virtual void CreateShaderVariable(const ComPtr<ID3D12Device>& device);

protected:
	virtual void CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	virtual void CreateShaderResourceView(const ComPtr<ID3D12Device>& device);

protected:
	UINT m_srvDescriptorSize;

	ComPtr<ID3D12DescriptorHeap>				m_srvDescriptorHeap;
	UINT										m_rootParameterIndex;
	vector<ComPtr<ID3D12Resource>>				m_textures;
	vector<ComPtr<ID3D12Resource>>				m_textureUploadBuffer;
};

