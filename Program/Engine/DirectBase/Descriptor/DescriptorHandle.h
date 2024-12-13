#pragma once
#include <d3d12.h>
#include <cstdint>

namespace DescriptorHandle {
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(ID3D12DescriptorHeap *const descriptorHeap, const uint32_t descriptorSize, const uint32_t index) {
		const D3D12_CPU_DESCRIPTOR_HANDLE handleCPUStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto handleCPU = handleCPUStart;
		handleCPU.ptr += (static_cast<uint64_t>(descriptorSize) * index);
		return handleCPU;
	}
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const D3D12_CPU_DESCRIPTOR_HANDLE handleCPUStart, const uint32_t descriptorSize, const uint32_t index) {
		auto handleCPU = handleCPUStart;
		handleCPU.ptr += (static_cast<uint64_t>(descriptorSize) * index);
		return handleCPU;
	}

	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(ID3D12DescriptorHeap *const descriptorHeap, const uint32_t descriptorSize, const uint32_t index) {
		const D3D12_GPU_DESCRIPTOR_HANDLE handleGPUStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		auto handleGPU = handleGPUStart;
		handleGPU.ptr += (static_cast<uint64_t>(descriptorSize) * index);
		return handleGPU;
	}
}