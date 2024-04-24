#pragma once
#include "stdafx.h"
#include "buffer.h"
#include "mesh.h"
#include "object.h"

struct InstanceData : public BufferBase
{
	XMFLOAT4X4 worldMatrix;
	UINT textureIndex;
};

class Instance
{
public:
	Instance(const ComPtr<ID3D12Device>& device, const shared_ptr<MeshBase>& mesh, UINT maxObjectCount);

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetTexture(const shared_ptr<Texture>& texture);
	void SetObject(const shared_ptr<InstanceObject>& object);
	void SetObjects(const vector<shared_ptr<InstanceObject>>& objects);
	void SetObjects(vector<shared_ptr<InstanceObject>>&& objects);

private:
	shared_ptr<MeshBase> m_mesh;
	shared_ptr<Texture>					m_texture;

	vector<shared_ptr<InstanceObject>> m_objects;
	UINT m_maxObjectCount;

	unique_ptr<UploadBuffer<InstanceData>> m_instanceBuffer;
};