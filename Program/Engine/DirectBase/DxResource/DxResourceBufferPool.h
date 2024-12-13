/// @file DxResourceBufferPool.h
/// @brief DxResourceの確保と保持を行うクラス
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"
#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include "../Base/EngineObject.h"
#include <vector>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

namespace SolEngine {

	template <size_t ElementSize = 1>
		requires (ElementSize != 0)
	class DxResourceBufferPool : public SoLib::Singleton<DxResourceBufferPool<ElementSize>>, protected EngineObject {
		friend SoLib::Singleton<DxResourceBufferPool>;
		DxResourceBufferPool() = default;
		DxResourceBufferPool(const DxResourceBufferPool &) = delete;
		DxResourceBufferPool &operator=(const DxResourceBufferPool &) = delete;
		~DxResourceBufferPool() = default;

		using Singleton = SoLib::Singleton<DxResourceBufferPool<ElementSize>>;
	public:

		/// @brief ハンドル
		/// @details 添え字とバージョンを持つ｡両方が一致した場合､同じリソースと見なす｡
		struct Handle {

			using ContainType = std::array<char, 0x100 * ElementSize>;

			Handle() = default;
			Handle(const Handle &) = default;
			Handle(Handle &&) = default;
			Handle &operator=(const Handle &r) { index_ = r.index_; version_ = r.version_; return *this; }

			Handle(const size_t index, const size_t version) : index_(index), version_(version) {};
			bool operator==(const Handle &) const = default;

			auto operator<=>(const Handle &that) const -> std::weak_ordering {

				const size_t aIndex = this->index_;
				const size_t bIndex = that.index_;

				// ハンドルが一致していない場合
				if (aIndex != bIndex) {
					return aIndex <=> bIndex;
				}
				// 一致していた場合
				else {
					return this->version_ <=> that.version_;
				}
			}
			/// @brief 添え字を取得
			/// @return 添え字
			size_t GetIndex() const { return index_; }
			/// @brief バージョンを取得
			/// @return バージョン
			size_t GetVersion() const { return version_; }
			ContainType *GetResource() { return IsActive() ? static_cast<ContainType *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }
			const ContainType *GetResource() const { return IsActive() ? static_cast<const ContainType *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }

			template <SoLib::IsRealType T>
			T *GetResource() { return IsActive() ? static_cast<T *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }
			template <SoLib::IsRealType T>
			const T *GetResource() const { return IsActive() ? static_cast<const T *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }

			inline ContainType *operator*() { return GetResource(); }
			inline const ContainType *operator*() const { return GetResource(); }

			/// @brief このデータが有効であるか
			/// @return 有効な場合true
			bool IsActive() const { return static_cast<bool>(*this); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const {
				return
					index_ != (std::numeric_limits<size_t>::max)() and 		// データが最大値(無効値)に設定されていないか
					Singleton::instance_ and 								// マネージャーが存在するか
					Singleton::instance_->version_ == version_ and 			// バージョンが同じか
					Singleton::instance_->size_ > index_ and 				// 参照ができる状態か
					Singleton::instance_->resources_.at(index_)->mapPtr_;	// データが存在するか
			}

		private:
			size_t index_ = (std::numeric_limits<size_t>::max)();
			size_t version_ = (std::numeric_limits<size_t>::max)();
		};
	public:
		/// @brief 初期化
		void Init();
		/// @brief 解放		
		void Clear();

		/// @brief データを追加
		/// @tparam T 追加するデータの型
		/// @param[in] data 追加するデータ
		/// @return 追加したデータのハンドル
		template <SoLib::IsRealType T>
		Handle PushBack(const T &data = {});

		/// @brief データを追加
		/// @tparam Itr 追加するデータのイテレータ
		/// @param[in] begin 追加するデータのイテレータの開始位置
		/// @param[in] end 追加するデータのイテレータの終了位置
		/// @return 追加したデータのハンドル群
		template<typename Itr>
		std::vector<Handle> PushBack(Itr begin, Itr end);

	private:

		class DxResourceBuffer {
		public:
			/// @brief リソースを作成
			/// @return 作成したリソース
			static std::unique_ptr<DxResourceBuffer> Create();

			void *mapPtr_ = nullptr;
			ComPtr<ID3D12Resource> resource_ = nullptr;
		};

		class DxResourceItem {
		public:
			DxResourceItem() = default;
			DxResourceItem(std::unique_ptr<DxResourceBuffer> element) : item_(std::move(element)) {}
			DxResourceItem &operator=(std::unique_ptr<DxResourceBuffer> element) { item_ = std::move(element); return *this; }

			template<SoLib::IsRealType T>
			DxResourceItem &operator=(T *element) { std::memcpy(item_->mapPtr_, element, sizeof(T)); return *this; }

			DxResourceBuffer *const operator->() { return item_.get(); }
			const DxResourceBuffer *const operator->() const { return item_.get(); }

			std::unique_ptr<DxResourceBuffer> item_ = nullptr;
		};

		size_t version_ = 0;
		size_t size_ = 0;

		std::vector<DxResourceItem> resources_;

	};

	template<size_t ElementSize>
		requires (ElementSize != 0)
	inline std::unique_ptr<typename DxResourceBufferPool<ElementSize>::DxResourceBuffer> DxResourceBufferPool<ElementSize>::DxResourceBuffer::Create()
	{
		// 書き込み先を作成
		std::unique_ptr<DxResourceBuffer> result = std::make_unique<DxResourceBuffer>();
		HRESULT hr = S_FALSE;
		// デバイスの取得
		auto *const device = EngineObject::GetDevice();

		// 256バイト単位のアライメント
		result->resource_ = CreateBufferResource(device, 0x100 * ElementSize);
		// リソースをマップ
		hr = result->resource_->Map(0, nullptr, reinterpret_cast<void **>(&result->mapPtr_));
		assert(SUCCEEDED(hr));

#ifdef _DEBUG
		// デバッグ用の名前を付ける
		result->resource_->SetName(L"DxResourcePool");

#endif // _DEBUG

		// リソースを返す
		return std::move(result);
	}

	template<size_t ElementSize>
		requires (ElementSize != 0)
	inline void DxResourceBufferPool<ElementSize>::Init()
	{
		version_ = 0;
		size_ = 0;
	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	inline void DxResourceBufferPool<ElementSize>::Clear()
	{
		size_ = 0;
		version_++;

	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	template<SoLib::IsRealType T>
	inline DxResourceBufferPool<ElementSize>::Handle DxResourceBufferPool<ElementSize>::PushBack(const T &data)
	{
		// 要素が足りなかったら延長する
		if (resources_.size() < size_ + 1) {
			resources_.push_back(DxResourceBuffer::Create());
		}

		// アドレスを代入する
		DxResourceItem &target = resources_[size_];

		// メモリコピー
		std::memcpy(target->mapPtr_, &data, sizeof(T));

		Handle result{ size_, version_ };

		// サイズを拡張
		size_++;

		return result;
	}
	template<size_t ElementSize>
		requires (ElementSize != 0)
	template<typename Itr>
	inline std::vector<typename DxResourceBufferPool<ElementSize>::Handle> DxResourceBufferPool<ElementSize>::PushBack(Itr begin, Itr end)
	{
		// イテレータの差分
		const size_t diff = std::distance(begin, end);

		// 要素が足りなかったら延長する
		if (resources_.size() < size_ + diff) { // 既存の領域より､追加後の領域の長さが大きかったら延長処理を行う
			resources_.resize(size_ + diff);
			std::generate_n(resources_.begin() + size_, diff, DxResourceBuffer::Create);
		}

		// メモリコピー
		std::transform(begin, end, &resources_[size_], [](const typename Itr::value_type &data)->const typename Itr::value_type *{ return &data; });

		std::vector<Handle> result{ diff };
		for (size_t i = 0; i < diff; i++) {
			result[i] = Handle{ i + size_, version_ };
		}

		// 領域を延長
		size_ += diff;
		return result;
	}

	/// @brief データがどのハンドルに割り当てられているかを返す
	/// @tparam T 調べたい型
	template<SoLib::IsRealType T>
	using DxResourcePoolBufferList = DxResourceBufferPool<(~0xffu & (sizeof(T) + 0xffu)) / 0x100>;
}