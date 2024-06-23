#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "../../Utils/SoLib/SoLib_Traits.h"
#include "../Base/EngineObject.h"
#include <vector>
#include <d3d12.h>

namespace SolEngine {

	template <size_t ElementSize = 1>
		requires (ElementSize != 0)
	class DxResourcePool : public SoLib::Singleton<DxResourcePool<ElementSize>>, protected EngineObject {
		friend SoLib::Singleton<DxResourcePool>;
		DxResourcePool() = default;
		DxResourcePool(const DxResourcePool &) = delete;
		DxResourcePool &operator=(const DxResourcePool &) = delete;
		~DxResourcePool() = default;
	public:

		void Init();

		void Clear();

		template <SoLib::IsRealType T>
		void PushBack(const T &data = {});

		template <SoLib::IsRealType T, SoLib::IsContainsType<T> C>
		void PushBack(const C &data);

	private:

		class DxResourceElement {
		public:

			static std::unique_ptr<DxResourceElement> Create();

			//private:
			void *mapPtr_;
			ComPtr<ID3D12Resource> resource_;
		};

		class DxResourceItem {
		public:
			DxResourceItem() = default;
			DxResourceItem(std::unique_ptr<DxResourceElement> element) : item_(std::move(element)) {}
			DxResourceItem &operator=(std::unique_ptr<DxResourceElement> element) { item_ = std::move(element); return *this; }

			template<SoLib::IsRealType T>
			DxResourceItem &operator=(T *element) { std::memcpy(item_->memPtr, element, sizeof(T)); return *this; }

			DxResourceElement *const operator->() { return item_.get(); }
			const DxResourceElement *const operator->() const { return item_.get(); }

			std::unique_ptr<DxResourceElement> item_;
		};

		size_t version_ = 0;
		size_t size_ = 0;

		std::vector<DxResourceItem> items_;

	};
	template<size_t ElementSize>
		requires (ElementSize != 0)
	inline std::unique_ptr<typename DxResourcePool<ElementSize>::DxResourceElement> DxResourcePool<ElementSize>::DxResourceElement::Create()
	{
		std::unique_ptr<DxResourceElement> result = std::make_unique<DxResourceElement>();
		HRESULT hr = S_FALSE;

		auto *const device = EngineObject::GetDevice();

		// 256バイト単位のアライメント
		result->resource_ = CreateBufferResource(device, 0xFF * ElementSize);

		hr = result->resource_->Map(0, nullptr, reinterpret_cast<void **>(&result->mapPtr_));
		assert(SUCCEEDED(hr));

#ifdef _DEBUG

		result->resource_->SetName(L"DxResourcePool");

#endif // _DEBUG

		return std::move(result);
	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	inline void DxResourcePool<ElementSize>::Init()
	{
		version_ = 0;
		size_ = 0;
	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	inline void DxResourcePool<ElementSize>::Clear()
	{
		size_ = 0;
		version_++;

	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	template<SoLib::IsRealType T>
	inline void DxResourcePool<ElementSize>::PushBack(const T &data)
	{
		// 要素が足りなかったら延長する
		if (items_.size() < size_ + 1) {
			items_.push_back(DxResourceElement::Create());
		}

		// アドレスを代入する
		DxResourceItem &target = items_[size_];

		// メモリコピー
		std::memcpy(target->mapPtr_, &data, sizeof(T));

		// サイズを拡張
		size_++;
	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	template<SoLib::IsRealType T, SoLib::IsContainsType<T> C>
	inline void DxResourcePool<ElementSize>::PushBack(const C &datas)
	{
		// 要素が足りなかったら延長する
		if (items_.size() < size_ + datas.size()) { // 既存の領域より､追加後の領域の長さが大きかったら延長処理を行う
			items_.resize(size_ + datas.size());
			std::for_each_n(items_.begin() + size_, datas.size(), [](DxResourceItem &item) { item = DxResourceElement::Create(); });
		}

		// メモリコピー
		std::transform(datas.begin(), datas.end(), &items_[size_], [](const T &data)->const T *{ return &data; });

	}

	template<SoLib::IsRealType T>
	using DxResourcePoolList = DxResourcePool<(~0xffu & (sizeof(T) + 0xffu)) / 0x100>;
}