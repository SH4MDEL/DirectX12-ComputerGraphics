#pragma once
#include "texture.h"

class ShadowMap : public Texture
{
public:
	ShadowMap(const ComPtr<ID3D12Device>& device, UINT width = 1024, UINT height = 1024);

	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void Close(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

private:
	void CreateTexture(const ComPtr<ID3D12Device>& device);
	void CreateShaderVariable(const ComPtr<ID3D12Device>& device) override;
	void CreateDsvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	void CreateDepthStencilView(const ComPtr<ID3D12Device>& device);

private:
	UINT							m_width;
	UINT							m_height;

	D3D12_VIEWPORT					m_viewport;
	D3D12_RECT						m_scissorRect;

	ComPtr<ID3D12DescriptorHeap>	m_dsvDescriptorHeap;
};

