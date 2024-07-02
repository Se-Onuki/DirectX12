#pragma once
#include <d3d12.h>
#include <cassert>
#include <cstdint>
#include <wrl.h>


Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device *device, const size_t &sizeBytes, const D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>CreateDescriptorHeap(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
