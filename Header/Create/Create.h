#pragma once
#include <d3d12.h>
#include <cassert>
#include <cstdint>

namespace Render {
	struct VertexData;
}

ID3D12Resource *CreateBufferResource(ID3D12Device *device, const size_t &sizeBytes);

_NODISCARD ID3D12DescriptorHeap *CreateDescriptorHeap(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

void CreateSphere(Render::VertexData *const vertex,
	ID3D12Resource *const indexResource, const uint32_t &subdivision);