#pragma once
#include <d3d12.h>
#include <cstdint>

namespace DescriptorHandIe {
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(ID3D12DescriptorHeap *const descriptorHeap, const uint32_t &descriptorSize, const uint32_t &index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (static_cast<uint64_t>(descriptorSize) * index);
		return handleCPU;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(ID3D12DescriptorHeap *const descriptorHeap, const uint32_t &descriptorSize, const uint32_t &index) {
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (static_cast<uint64_t>(descriptorSize) * index);
		return handleGPU;
	}
}