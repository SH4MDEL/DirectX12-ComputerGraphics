#pragma once
#include "stdafx.h"

class Texture
{
public:
	Texture() = delete;
	Texture(const ComPtr<ID3D12Device>& device, 
		const ComPtr<ID3D12GraphicsCommandList>& commandList, 
		const wstring& fileName, UINT rootParameterIndex);
	~Texture() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void ReleaseUploadBuffer();

private:
	void LoadTexture(const ComPtr<ID3D12Device>& device,
		const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName);
	void CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	void CreateShaderResourceView(const ComPtr<ID3D12Device>& device);


private:
	ComPtr<ID3D12DescriptorHeap>	m_srvDescriptorHeap;
	ComPtr<ID3D12Resource>			m_texture;
	ComPtr<ID3D12Resource>			m_textureUploadBuffer;
	UINT							m_rootParameterIndex;
};

