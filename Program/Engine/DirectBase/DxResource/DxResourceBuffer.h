#pragma once
#include "../Base/EngineObject.h"
#include <d3d12.h>
#include <algorithm>
#include <memory>
#include <span>
#include <bitset>

namespace SolEngine {

	template<D3D12_HEAP_TYPE HType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD>
	class DxResourceBuffer : protected EngineObject {
	public:

		DxResourceBuffer() = default;
		DxResourceBuffer(const DxResourceBuffer &) = delete;
		DxResourceBuffer &operator=(const DxResourceBuffer &) = delete;

		DxResourceBuffer(DxResourceBuffer &&other) noexcept : resource_(std::move(other.resource_)), itemData_(other.itemData_), cbView_(other.cbView_) {}
		DxResourceBuffer &operator=(DxResourceBuffer &&other) noexcept { resource_ = std::move(other.resource_); itemData_ = std::move(other.itemData_); cbView_ = other.cbView_; return *this; }

		bool operator==(const DxResourceBuffer &other) const = default;

		// CPUアクセスメモリを持っているか
		constexpr static bool kHasMemory_ = HType != D3D12_HEAP_TYPE_DEFAULT;

		using MemType = std::array<char, 0x100>;

	public:

	public:

		/// @brief データを構築する
		/// @param dataSize sizeof()で得られたクラスのサイズ
		/// @param dataCount データの数
		/// @return 生成されたデータ
		static DxResourceBuffer Create(const uint32_t dataSize, const uint32_t dataCount = 1);

	public:

		MemType *data() const noexcept requires(kHasMemory_) { return itemData_.data(); }
		std::span<MemType>::iterator begin() const noexcept requires(kHasMemory_) { return itemData_.begin(); }
		std::span<MemType>::iterator end() const noexcept requires(kHasMemory_) { return itemData_.end(); }
		size_t size() const noexcept {
			if constexpr (kHasMemory_) { return itemData_.size(); }
			else { return itemData_; }
		}

		template<typename T>
			requires(kHasMemory_)
		std::span<T> GetAccessor() {
			// アイテムのサイズ
			constexpr size_t typeSize = sizeof(T);
			// メモリ量
			const size_t memSize = sizeof(MemType) * size();
			// 配列の要素数
			const size_t tItemCount = memSize / typeSize;
			// そのデータが何個格納できるかを計算する
			std::span<T> result{ reinterpret_cast<T *>(itemData_.data()), tItemCount };

			return result;
		}

		template<typename T>
			requires(kHasMemory_)
		const std::span<T> GetAccessor() const {
			// アイテムのサイズ
			constexpr size_t typeSize = sizeof(T);
			// メモリ量
			const size_t memSize = sizeof(MemType) / size();
			// 配列の要素数
			const size_t tItemCount = memSize / typeSize;
			// そのデータが何個格納できるかを計算する
			std::span<T> result{ reinterpret_cast<T *>(itemData_.data()), tItemCount };

			return result;
		}

		ID3D12Resource *GetResource() noexcept { return resource_.Get(); }
		ID3D12Resource *GetResource() const noexcept { return resource_.Get(); }

		const D3D12_CONSTANT_BUFFER_VIEW_DESC &GetCBView() const noexcept { return cbView_; }

	private:

		ComPtr<ID3D12Resource> resource_;

		std::conditional_t<kHasMemory_, std::span<MemType>, size_t> itemData_;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbView_;

	};

#pragma region Function

	template<D3D12_HEAP_TYPE HType>
	DxResourceBuffer<HType> DxResourceBuffer<HType>::Create(const uint32_t dataSize, const uint32_t dataCount)
	{
		ID3D12Device *const device = GetDevice();

		DxResourceBuffer result;

		// 確保するメモリ量｡ (256バイト単位でアライメント)
		const size_t memSize = (static_cast<size_t>(dataSize) * dataCount + 0xffllu) & ~0xffllu;

		// メモリを構築
		result.resource_ = CreateBufferResource(device, memSize, HType);

		HRESULT hr = S_FALSE;


		if constexpr (kHasMemory_) {
			void *tmp{};

			hr = result.resource_->Map(0, nullptr, &tmp);

			// データを渡す
			result.itemData_ = { reinterpret_cast<MemType *>(tmp), memSize >> 8u };
		}
		else {

			result.itemData_ = memSize >> 8u;
		}

		assert(SUCCEEDED(hr));

		// ビューの構築
		result.cbView_ = { .BufferLocation = result.resource_->GetGPUVirtualAddress(), .SizeInBytes = static_cast<uint32_t>(result.resource_->GetDesc().Width) };

		return result;
	}

#pragma endregion

}