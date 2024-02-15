#pragma once
#include "stdafx.h"

class Shader
{
public:
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Shader() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

protected:
	ComPtr<ID3D12PipelineState> m_pipelineState;
};

