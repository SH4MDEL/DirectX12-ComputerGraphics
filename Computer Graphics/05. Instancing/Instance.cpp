#include "Instance.h"

Instance::Instance(const ComPtr<ID3D12Device>& device, const shared_ptr<MeshBase>& mesh, UINT maxObjectCount)
	: m_mesh{ mesh }, m_maxObjectCount{ maxObjectCount }
{
	m_instanceBuffer = make_unique<UploadBuffer<InstanceData>>(device,
		(UINT)RootParameter::Instance, m_maxObjectCount, false);
}

void Instance::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	UpdateShaderVariable(commandList);

	m_mesh->Render(commandList, m_objects.size());
}

void Instance::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (int i = 0; const auto & object : m_objects) {
		InstanceData buffer;
		object->UpdateShaderVariable(buffer);
		m_instanceBuffer->Copy(buffer, i++);
	}
	m_instanceBuffer->UpdateRootShaderResource(commandList);
	if (m_texture) m_texture->UpdateShaderVariable(commandList);
}

void Instance::SetTexture(const shared_ptr<Texture>& texture)
{
	m_texture = texture;
}

void Instance::SetObject(const shared_ptr<InstanceObject>& object)
{
	if (m_objects.size() == m_maxObjectCount) return;
	m_objects.push_back(object);
}

void Instance::SetObjects(const vector<shared_ptr<InstanceObject>>& objects)
{
	m_objects = objects;
}

void Instance::SetObjects(vector<shared_ptr<InstanceObject>>&& objects)
{
	m_objects = move(objects);
}