/// @file DxResourceBufferPoolManager.h
/// @brief DxResourceのバッファの一括管理を行う
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"
#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include "../Base/EngineObject.h"

#include <deque>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <unordered_set>
#include <vector>

#include <d3d12.h>
#include <wrl.h>

#include "DxResourceBuffer.h"

namespace SolEngine {

	template <D3D12_HEAP_TYPE HType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD>
	class DxResourceBufferPoolManager : public SoLib::Singleton<DxResourceBufferPoolManager<HType>>, protected EngineObject
	{
		friend SoLib::Singleton<DxResourceBufferPoolManager<HType>>;
		DxResourceBufferPoolManager() = default;
		DxResourceBufferPoolManager(const DxResourceBufferPoolManager &) = delete;
		DxResourceBufferPoolManager &operator=(const DxResourceBufferPoolManager &) = delete;
		~DxResourceBufferPoolManager() = default;

		using Singleton = SoLib::Singleton<DxResourceBufferPoolManager<HType>>;

	public:
		// CPUアクセスメモリを持っているか
		constexpr static bool kHasMemory_ = HType != D3D12_HEAP_TYPE_DEFAULT;

	private:
		/// @class DxResourceItem
		/// @brief DxResourceBufferの単体データ
		class DxResourceItem
		{
		public:
			DxResourceItem() = default;
			DxResourceItem(std::unique_ptr<DxResourceBuffer<HType>> element) : item_(std::move(element)) {}
			DxResourceItem &operator=(std::unique_ptr<DxResourceBuffer<HType>> element)
			{
				item_ = std::move(element);
				return *this;
			}

			DxResourceItem(const DxResourceItem &) = delete;
			DxResourceItem &operator=(const DxResourceItem &) = delete;

			DxResourceItem(DxResourceItem &&) = default;
			DxResourceItem &operator=(DxResourceItem &&) = default;

			template <SoLib::IsRealType T>
			DxResourceItem &operator=(const T *element)
				requires(kHasMemory_)
			{
				std::memcpy(item_->data(), element, sizeof(T));
				return *this;
			}
			bool operator==(const DxResourceItem &) const = default;
			bool operator==(const DxResourceBuffer<HType> *that) const { return item_.get() == that; }

			DxResourceBuffer<HType> *const operator->()
				requires(kHasMemory_)
			{
				return item_.get();
			}
			const DxResourceBuffer<HType> *const operator->() const
				requires(kHasMemory_)
			{
				return item_.get();
			}

			std::unique_ptr<DxResourceBuffer<HType>> item_ = nullptr;
		};

		using ResourceList = std::list<DxResourceItem>;

	public:
		/// @class UniqueHandle
		/// @brief DxResourceBufferの単体データを管理するハンドル
		/// @details ハンドルが破棄された場合､自動的に返却される
		struct UniqueHandle
		{
			UniqueHandle() = default;
			UniqueHandle(const UniqueHandle &) = delete;
			UniqueHandle &operator=(const UniqueHandle &) = delete;
			UniqueHandle(UniqueHandle &&that)
			{
				if (*this) {
					Singleton::instance_->Release(std::move(*item_));
				}
				item_ = that.item_;
				that.item_ = Singleton::instance_->resources_.end();
			}
			UniqueHandle &operator=(UniqueHandle &&that) noexcept
			{
				if (*this) {
					Singleton::instance_->Release(std::move(*item_));
				}
				item_ = that.item_;
				that.item_ = Singleton::instance_->resources_.end();
				return *this;
			}

			UniqueHandle(const ResourceList::iterator &itr) { item_ = itr; }

			~UniqueHandle()
			{
				if (IsActive()) {
					Singleton::instance_->Release(std::move(*item_));
				}
			}

			bool operator==(const UniqueHandle &) const = default;

			DxResourceBuffer<HType> *GetResource() { return IsActive() ? (*item_)->item_.get() : nullptr; }
			const DxResourceBuffer<HType> *GetResource() const { return IsActive() ? (*item_)->item_.get() : nullptr; }

			inline DxResourceBuffer<HType> *operator->() { return GetResource(); }
			inline const DxResourceBuffer<HType> *operator->() const { return GetResource(); }

			inline DxResourceBuffer<HType> *operator*() { return GetResource(); }
			inline const DxResourceBuffer<HType> *operator*() const { return GetResource(); }

			bool IsActive() const { return static_cast<bool>(*this); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const
			{
				return Singleton::instance_ and							 // マネージャーが存在するか
					   item_ and										 // データが存在するか
					   *item_ != Singleton::instance_->resources_.end(); // データが存在するか
			}

		private:
			friend DxResourceBufferPoolManager<HType>;

			std::optional<typename ResourceList::iterator> item_ = std::nullopt;
		};

	public:
		/// @brief 初期化
		void Init();

		/// @brief リソースの破棄
		/// @param[in] itr リソースのイテレータ
		void Release(ResourceList::iterator &&itr);

		/// @brief データの追加
		/// @tparam T 追加する型
		/// @param[in] size 追加する量
		/// @return 追加したデータのハンドル
		template <SoLib::IsRealType T>
		UniqueHandle PushBack(uint32_t size = 1u);
		
		/// @brief データの追加
		/// @tparam T 追加する型
		/// @param[in] data 追加するデータ
		/// @return 追加したデータのハンドル
		template <SoLib::IsRealType T>
		UniqueHandle PushBack(const T &data);

		/// @brief データの追加
		/// @tparam Itr イテレータの型
		/// @param begin 開始イテレータ
		/// @param end 番兵イテレータ
		/// @return 追加したデータのハンドル群
		template <typename Itr>
		std::vector<DxResourceBuffer<HType> *> PushBack(Itr begin, Itr end);

	private:
		// リソースのマネージャ
		ResourceList resources_;

		// 使っていないリソース [メモリサイズ, データの配列]
		std::map<uint32_t, std::deque<DxResourceItem>> unUsingResource_;
	};

	template <D3D12_HEAP_TYPE HType>
	inline void DxResourceBufferPoolManager<HType>::Init()
	{
	}

	template <D3D12_HEAP_TYPE HType>
	inline void DxResourceBufferPoolManager<HType>::Release(ResourceList::iterator &&itr)
	{

		// もしそのデータが存在してなかったら終わり
		if (itr == resources_.end()) {
			return;
		}

		// データのサイズ
		const uint32_t size = static_cast<uint32_t>(itr->item_->size());

		// ポインタを渡して構築する
		unUsingResource_[size].emplace_back(std::move(itr->item_));

		// データを破棄
		resources_.erase(itr);
	}

	template <D3D12_HEAP_TYPE HType>
	template <SoLib::IsRealType T>
	inline DxResourceBufferPoolManager<HType>::UniqueHandle DxResourceBufferPoolManager<HType>::PushBack(uint32_t size)
	{
		// 構築する要素
		DxResourceItem result{};

		// 追加するデータのメモリサイズ
		const uint32_t itemSize = ((sizeof(T) * size) + 0xff) >> 8u;

		// 獲得する元の配列
		auto mapItr = unUsingResource_.find(itemSize);

		// 要素が存在したら､それを借りる
		if (mapItr != unUsingResource_.end() and mapItr->second.size() != 0u) { // 配列が存在して､データが余ってたら実行
			// 保存するデータ
			result = std::move(mapItr->second.front()); // データの先頭を取得する
			mapItr->second.pop_front();					// データの先頭を破棄する
		}
		// 要素が足りなかったら直接生成する
		else {
			// 保存するデータ
			result.item_ = std::make_unique<DxResourceBuffer<HType>>(DxResourceBuffer<HType>::Create(sizeof(T), size));
		}

		// そのデータを追加する
		resources_.push_back(std::move(result));

		auto itr = std::prev(resources_.end());

		// そのデータのポインタを返す
		return UniqueHandle(itr);
	}

	template <D3D12_HEAP_TYPE HType>
	template <SoLib::IsRealType T>
	inline DxResourceBufferPoolManager<HType>::UniqueHandle DxResourceBufferPoolManager<HType>::PushBack(const T &data)
	{
		UniqueHandle &&result = std::move(PushBack<T>());
		result->GetAccessor<T>()[0] = data;
		return std::move(result);
	}

	template <D3D12_HEAP_TYPE HType>
	template <typename Itr>
	inline std::vector<DxResourceBuffer<HType> *> DxResourceBufferPoolManager<HType>::PushBack(Itr begin, Itr end)
	{
		// イテレータの差分
		const size_t diff = std::distance(begin, end);

		// 構築する要素
		std::vector<DxResourceItem> target{diff};

		// 使用する型
		using Type = decltype(begin.operator*());

		// 追加するデータのメモリサイズ
		constexpr uint32_t itemSize = (sizeof(decltype(begin.operator*())) + 0xff) >> 8u;

		// 獲得する元の配列
		auto mapItr = unUsingResource_.find(itemSize);

		// すでに完了した数
		size_t successCount = 0u;

		// 要素が存在したら､それを借りる
		if (mapItr != unUsingResource_.end() and mapItr->second.size() != 0u) { // 配列が存在して､データが余ってたら実行

			// 配列からいくつ取得するか
			const size_t getCount = (std::min)(mapItr->second.size(), diff);
			std::transform(std::make_move_iterator(mapItr->second.rbegin()), std::make_move_iterator(mapItr->second.rbegin() + getCount), target.begin(), [](DxResourceItem &&item) { return std::move(item); });
			mapItr->second.erase(mapItr->second.end(), mapItr->second.end() - getCount);

			// 埋めた数を保存
			successCount = getCount;
		}
		// 要素が足りなかったら直接生成する
		if (successCount != diff) {
			std::for_each(target.begin() + successCount, target.end(), [](DxResourceItem &item) { return std::make_unique<DxResourceBuffer<HType>>(DxResourceBuffer<HType>::Create(itemSize * 0x100)); });
		}

		// データを渡す
		std::transform(begin, end, target.begin(), [](const Type &item) { return &item; });

		std::vector<DxResourceBuffer<HType> *> result{diff};
		std::transform(target.begin(), target.end(), result.begin(), [](const DxResourceItem &item) -> const DxResourceBuffer<HType> * { return item.item_.get(); });

		// そのデータを追加する
		resources_.insert(std::make_move_iterator(target.begin()), std::make_move_iterator(target.end()));

		// そのデータのポインタを返す
		return result;
	}

}