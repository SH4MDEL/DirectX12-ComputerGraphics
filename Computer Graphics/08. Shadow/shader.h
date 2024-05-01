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

class TerrainShader : public Shader
{
public:
	TerrainShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~TerrainShader() override = default;
};

class BillboardShader : public Shader
{
public:
	BillboardShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~BillboardShader() override = default;
};

class ObjectShadowShader : public Shader
{
public:
	ObjectShadowShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~ObjectShadowShader() override = default;
};

class BillboardShadowShader : public Shader
{
public:
	BillboardShadowShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~BillboardShadowShader() override = default;
};

class TerrainShadowShader : public Shader
{
public:
	TerrainShadowShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~TerrainShadowShader() override = default;
};