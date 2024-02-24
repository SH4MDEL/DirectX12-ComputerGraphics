#pragma once
#include "stdafx.h"

class Shader abstract
{
public:
	Shader() = default;
	virtual ~Shader() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

protected:
	ComPtr<ID3D12PipelineState> m_pipelineState;
};

class ObjectShader : public Shader
{
public:
	ObjectShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~ObjectShader() override = default;
};

class SkyboxShader : public Shader
{
public:
	SkyboxShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~SkyboxShader() override = default;
};