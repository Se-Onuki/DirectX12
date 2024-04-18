#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <stdint.h>

//#include "../Base/DirectXCommon.h"
#include "../Create/Create.h"

#include "DescriptorHandle.h"
#include "../Base/MemoryUsageManager.h"

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
class DescHeap {
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct Handle {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;
	};

	inline static const D3D12_DESCRIPTOR_HEAP_TYPE HeapType_ = HeapType;

	/// @brief デスクリプタヒープの使用領域
	struct HeapRange {
		HeapRange() = default;
		//HeapRange(uint32_t offset, uint32_t width, DescHeap *descHeap) : offset_(offset), width_(width), descHeap_(descHeap) {}
		HeapRange(std::shared_ptr<MemoryUsageManager::MemoryRange> &memoryRange, DescHeap *descHeap) : memoryRange_(memoryRange), descHeap_(descHeap) {}
		~HeapRange();
		std::shared_ptr<MemoryUsageManager::MemoryRange> memoryRange_;

		operator bool() const { return static_cast<bool>(memoryRange_); }

		/// @brief デスクリプタハンドルを取得する
		/// @param index 添え字
		/// @return デスクリプタハンドル
		Handle GetHandle(const uint32_t index) const;
	private:
		DescHeap *descHeap_ = nullptr;// 使用しているデスクリプタヒープのアドレス
	};

	DescHeap(ID3D12Device *const device, uint32_t size, bool shaderVisible = true);
	DescHeap(const DescHeap &) = delete;
	DescHeap &operator=(const DescHeap &) = delete;
	~DescHeap() = default;

	/// @brief コマンドリストにデスクリプタヒープを登録する
	/// @param commandList コマンドリスト
	/// @param numDescriptorHeaps 登録する番号
	void SetCommand(ID3D12GraphicsCommandList *const commandList, uint32_t numDescriptorHeaps);

	/// @brief デスクリプタヒープを取得する
	/// @return デスクリプタヒープのポインタ
	auto *const GetHeap() const { return descriptorHeap_.Get(); }

	/// @brief ハンドルを取得する
	/// @param offset 始点
	/// @param index 添え字
	/// @return デスクリプタハンドル
	Handle GetHandle(const uint32_t offset, const uint32_t index);

	/// @brief デスクリプタの使用予約
	/// @param width 使用量
	/// @return デスクリプタヒープの使用領域
	HeapRange RequestHeapAllocation(uint32_t width);


private:
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	const uint32_t heapSize_;
	uint32_t itemSize_;

	MemoryUsageManager memoryManager_;

	bool isShaderVisible_;
};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
DescHeap<HeapType>::DescHeap(ID3D12Device *const device, uint32_t size, bool shaderVisible) : heapSize_(size), memoryManager_(size) {
	itemSize_ = device->GetDescriptorHandleIncrementSize(HeapType);
	isShaderVisible_ = shaderVisible;

	descriptorHeap_ = CreateDescriptorHeap(device, HeapType, heapSize_, shaderVisible);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline void DescHeap<HeapType>::SetCommand(ID3D12GraphicsCommandList *const commandList, uint32_t numDescriptorHeaps) {
	ID3D12DescriptorHeap *descriptorHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(numDescriptorHeaps, descriptorHeaps);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline  DescHeap<HeapType>::Handle DescHeap<HeapType>::GetHandle(const uint32_t offset, const uint32_t index) {
	const uint32_t handle = offset + index;
	Handle heapHandle{};
	heapHandle.cpuHandle_ = DescriptorHandle::GetCPUHandle(descriptorHeap_.Get(), itemSize_, handle);
	if (isShaderVisible_) {
		heapHandle.gpuHandle_ = DescriptorHandle::GetGPUHandle(descriptorHeap_.Get(), itemSize_, handle);
	}
	return heapHandle;
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline DescHeap<HeapType>::HeapRange DescHeap<HeapType>::RequestHeapAllocation(uint32_t width) {
	auto memoryRange{ memoryManager_.RequestRange(width) };
	if (not memoryRange) {
		return HeapRange{};
	}
	HeapRange output{ memoryRange, this };
	return output;
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline DescHeap<HeapType>::Handle DescHeap<HeapType>::HeapRange::GetHandle(const uint32_t index) const {
	assert(memoryRange_->length > index && "与えられた添え字が、許可された範囲をオーバーしました");
	return descHeap_->GetHandle(memoryRange_->offset, index);
}

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
inline DescHeap<HeapType>::HeapRange::~HeapRange() {
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

/// @brief SRVやCBVなどのデスクリプタヒープ
using DescHeapCbvSrvUav = DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;