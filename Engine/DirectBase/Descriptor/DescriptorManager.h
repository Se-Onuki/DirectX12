#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <stdint.h>

#include "../Base/DirectXCommon.h"
#include "../Create/Create.h"

#include "DescriptorHandle.h"


template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t Size>
class DescHeap {
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct Handle {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;
	};


	DescHeap();
	~DescHeap() = default;

	void SetHeaps(ID3D12GraphicsCommandList *const commandList, uint32_t numDescriptorHeaps);

	auto *const GetHeap() const { return descriptorHeap_.Get(); }

	Handle GetHandle(const uint32_t offset, const uint32_t index);


private:
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	const uint32_t heapSize_ = Size;
	uint32_t itemSize_;
};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t Size>
DescHeap<HeapType, Size>::DescHeap() {
	auto *const device = DirectXCommon::GetInstance()->GetDevice();
	itemSize_ = device->GetDescriptorHandleIncrementSize(HeapType);

	descriptorHeap_ = CreateDescriptorHeap(device, HeapType, Size, true);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t Size>
inline void DescHeap<HeapType, Size>::SetHeaps(ID3D12GraphicsCommandList *const commandList, uint32_t numDescriptorHeaps) {
	ID3D12DescriptorHeap *descriptorHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(numDescriptorHeaps, descriptorHeaps);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t Size>
inline DescHeap<HeapType, Size>::Handle DescHeap<HeapType, Size>::GetHandle(const uint32_t offset, const uint32_t index) {
	const uint32_t handle = offset + index;
	DescriptorHandle::GetCPUHandle(descriptorHeap_.GetHeap(), itemSize_, handle);
	DescriptorHandle::GetGPUHandle(descriptorHeap_.GetHeap(), itemSize_, handle);
}

class DescriptorManager {

};