#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <stdint.h>

//#include "../Base/DirectXCommon.h"
#include "../Create/Create.h"

#include "DescriptorHandle.h"

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
class DescHeap {
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct Handle {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;
	};
	struct HeapRange {
		HeapRange(uint32_t offset, uint32_t width) : offset_(offset), width_(width) {}
		const uint32_t offset_;
		const uint32_t width_;
	};

	DescHeap(ID3D12Device *const device, uint32_t size);
	DescHeap(const DescHeap &) = delete;
	DescHeap &operator=(const DescHeap &) = delete;
	~DescHeap() = default;

	void SetCommand(ID3D12GraphicsCommandList *const commandList, uint32_t numDescriptorHeaps);

	auto *const GetHeap() const { return descriptorHeap_.Get(); }

	Handle GetHandle(const uint32_t offset, const uint32_t index);

	HeapRange RequestHeapAllocation(uint32_t width);


private:
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	const uint32_t heapSize_;
	uint32_t itemSize_;

	uint32_t usingIndex_ = 0u;
};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
DescHeap<HeapType>::DescHeap(ID3D12Device *const device, uint32_t size) : heapSize_(size) {
	itemSize_ = device->GetDescriptorHandleIncrementSize(HeapType);

	descriptorHeap_ = CreateDescriptorHeap(device, HeapType, heapSize_, true);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline void DescHeap<HeapType>::SetCommand(ID3D12GraphicsCommandList *const commandList, uint32_t numDescriptorHeaps) {
	ID3D12DescriptorHeap *descriptorHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(numDescriptorHeaps, descriptorHeaps);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline  DescHeap<HeapType>::Handle DescHeap<HeapType>::GetHandle(const uint32_t offset, const uint32_t index) {
	const uint32_t handle = offset + index;
	Handle heapHandle;
	heapHandle.cpuHandle_ = DescriptorHandle::GetCPUHandle(descriptorHeap_.GetHeap(), itemSize_, handle);
	heapHandle.gpuHandle_ = DescriptorHandle::GetGPUHandle(descriptorHeap_.GetHeap(), itemSize_, handle);
	return heapHandle;
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline DescHeap<HeapType>::HeapRange DescHeap<HeapType>::RequestHeapAllocation(uint32_t width) {
	HeapRange output{ usingIndex_, width };
	usingIndex_ += width;
	return output;
}

//template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
//class DescriptorManager {
//	DescriptorManager() = default;
//	DescriptorManager(const DescriptorManager &) = delete;
//	DescriptorManager &operator=(const DescriptorManager &) = delete;
//	~DescriptorManager() = default;
//public:
//	static auto *const GetInstance() {
//		static DescriptorManager instance{};
//		return &instance;
//	}
//
//	auto *const GetHeap() const {
//		&descriptorHeap_;
//	}
//
//
//private:
//
//	DescHeap<HeapType, > descriptorHeap_{};
//
//};