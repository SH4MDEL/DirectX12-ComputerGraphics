#pragma once
#include "stdafx.h"

struct BufferBase {};

template <typename T> requires derived_from<T, BufferBase>
class UploadBuffer
{
public:
	UploadBuffer(const ComPtr<ID3D12Device>& device, BOOL isConstantBuffer = false, UINT count = 1);
	~UploadBuffer();

    ID3D12Resource* Resource() const;
    void Copy(const T& data, UINT index = 0);

private:
    ComPtr<ID3D12Resource>  m_uploadBuffer;
    T*                      m_data;
    UINT                    m_byteSize;
    BOOL                    m_isConstantBuffer;

};

template<typename T> requires derived_from<T, BufferBase>
inline UploadBuffer<T>::UploadBuffer(const ComPtr<ID3D12Device>& device, 
    BOOL isConstantBuffer, UINT count) : m_isConstantBuffer{isConstantBuffer}
{
    if (m_isConstantBuffer) m_byteSize = ((sizeof(T) + 255) & ~255);
    else m_byteSize = sizeof(T);

    Utiles::ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(m_byteSize * count),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uploadBuffer)));

    Utiles::ThrowIfFailed((m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_data))));
}

template<typename T> requires derived_from<T, BufferBase>
inline UploadBuffer<T>::~UploadBuffer()
{
    if (m_uploadBuffer) m_uploadBuffer->Unmap(0, nullptr);
    m_data = nullptr;
}

template<typename T> requires derived_from<T, BufferBase>
inline ID3D12Resource* UploadBuffer<T>::Resource() const
{
    return m_uploadBuffer.Get();
}

template<typename T> requires derived_from<T, BufferBase>
inline void UploadBuffer<T>::Copy(const T& data, UINT index)
{
    memcpy(m_data + m_byteSize * index, &data, sizeof(T));
}