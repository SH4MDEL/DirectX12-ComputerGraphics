#pragma once
#include "stdafx.h"
#include "buffer.h"
#include "mesh.h"
#include "object.h"

struct InstanceData : public BufferBase 
{
	XMFLOAT4X4 worldMatrix;
};

template <typename T> requires derived_from<T, MeshBase>
class Instance
{
public:
	Instance(const ComPtr<ID3D12Device>& device, const shared_ptr<T>& mesh, UINT maxObjectCount);

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetObject(const shared_ptr<GameObject>& object);
	void SetObjects(const vector<shared_ptr<GameObject>>& objects);
	void SetObjects(vector<shared_ptr<GameObject>>&& objects);

private:
	shared_ptr<T> m_mesh;
	vector<shared_ptr<GameObject>> m_objects;
	UINT m_maxObjectCount;

	unique_ptr<UploadBuffer<InstanceData>> m_instanceBuffer;
};

template<typename T> requires derived_from<T, MeshBase>
inline Instance<T>::Instance(const ComPtr<ID3D12Device>& device, const shared_ptr<T>& mesh, UINT maxObjectCount)
	: m_mesh{ mesh }, m_maxObjectCount { maxObjectCount }
{
	m_instanceBuffer = make_unique<UploadBuffer<InstanceData>>(device, 
		(UINT)RootParameter::Instance, m_maxObjectCount, false);
}

template<typename T> requires derived_from<T, MeshBase>
inline void Instance<T>::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (int i = 0; const auto object : m_objects) {
		InstanceData buffer;
		XMStoreFloat4x4(&buffer.worldMatrix,
			XMMatrixTranspose(XMLoadFloat4x4(&object->GetWorldMatrix())));
		m_instanceBuffer->Copy(buffer, i++);
	}
	m_instanceBuffer->UpdateRootShaderResource(commandList);

	m_mesh->Render(commandList, m_objects.size());
}

template<typename T> requires derived_from<T, MeshBase>
inline void Instance<T>::SetObject(const shared_ptr<GameObject>& object)
{
	if (m_objects.size() == m_maxObjectCount) return;
	m_objects.push_back(object);
}

template<typename T> requires derived_from<T, MeshBase>
inline void Instance<T>::SetObjects(const vector<shared_ptr<GameObject>>& objects)
{
	m_objects = objects;
}

template<typename T> requires derived_from<T, MeshBase>
inline void Instance<T>::SetObjects(vector<shared_ptr<GameObject>>&& objects)
{
	m_objects = move(objects);
}
