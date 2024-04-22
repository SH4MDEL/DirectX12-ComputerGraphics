#include "material.h"

void Material::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (int i = 0; const auto& material : m_material) {
		m_constantBuffer->Copy(material, i++);
	}
	m_constantBuffer->UpdateRootConstantBuffer(commandList);
}

void Material::SetMaterial(MaterialData material)
{
	m_material.push_back(material);
}

void Material::SetMaterial(XMFLOAT3 fresnelR0, FLOAT roughness)
{
	MaterialData material;
	material.fresnelR0 = fresnelR0;
	material.roughness = roughness;
	m_material.push_back(material);
}

void Material::CreateShaderVariable(const ComPtr<ID3D12Device>& device)
{
	m_constantBuffer = make_unique<UploadBuffer<MaterialData>>(device, 
		static_cast<UINT>(RootParameter::Material), static_cast<UINT>(m_material.size()), true);
}

