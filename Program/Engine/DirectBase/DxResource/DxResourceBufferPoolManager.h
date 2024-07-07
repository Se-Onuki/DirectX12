#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "../../Utils/SoLib/SoLib_Traits.h"
#include "../Base/EngineObject.h"

#include <vector>
#include <memory>
#include <deque>
#include <unordered_set>

#include <d3d12.h>
#include <wrl.h>

#include "DxResourceBuffer.h"

namespace SolEngine {

	template<D3D12_HEAP_TYPE HType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD>
	class DxResourceBufferPoolManager : public SoLib::Singleton<DxResourceBufferPoolManager<HType>>, protected EngineObject {
		friend SoLib::Singleton<DxResourceBufferPoolManager<HType>>;
		DxResourceBufferPoolManager() = default;
		DxResourceBufferPoolManager(const DxResourceBufferPoolManager &) = delete;
		DxResourceBufferPoolManager &operator=(const DxResourceBufferPoolManager &) = delete;
		~DxResourceBufferPoolManager() = default;

		using Singleton = SoLib::Singleton<DxResourceBufferPoolManager<HType>>;
	public:

		// CPUアクセスメモリを持っているか
		constexpr static bool kHasMemory_ = HType != D3D12_HEAP_TYPE_DEFAULT;

		//struct Handle {

		//	using ContainType = std::array<char, 0x100>;

		//	Handle() = default;
		//	Handle(const Handle &) = default;
		//	Handle(Handle &&) = default;
		//	Handle &operator=(const Handle &r) { index_ = r.index_; version_ = r.version_; return *this; }

		//	Handle(const size_t index, const size_t version) : index_(index), version_(version) {};
		//	bool operator==(const Handle &) const = default;

		//	auto operator<=>(const Handle &that) const -> std::weak_ordering {

		//		const size_t aIndex = this->index_;
		//		const size_t bIndex = that.index_;

		//		// ハンドルが一致していない場合
		//		if (aIndex != bIndex) {
		//			return aIndex <=> bIndex;
		//		}
		//		// 一致していた場合
		//		else {
		//			return this->version_ <=> that.version_;
		//		}
		//	}

		//	size_t GetIndex() const { return index_; }
		//	size_t GetVersion() const { return version_; }
		//	ContainType *GetResource() { return IsActive() ? static_cast<ContainType *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }
		//	const ContainType *GetResource() const { return IsActive() ? static_cast<const ContainType *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }

		//	template <SoLib::IsRealType T>
		//	T *GetResource() { return IsActive() ? static_cast<T *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }
		//	template <SoLib::IsRealType T>
		//	const T *GetResource() const { return IsActive() ? static_cast<const T *>(Singleton::instance_->resources_.at(index_)->mapPtr_) : nullptr; }

		//	inline ContainType *operator*() { return GetResource(); }
		//	inline const ContainType *operator*() const { return GetResource(); }

		//	bool IsActive() const { return static_cast<bool>(*this); }

		//	/// @brief このデータが有効であるか
		//	explicit inline operator bool() const {
		//		return
		//			index_ != (std::numeric_limits<size_t>::max)() and 		// データが最大値(無効値)に設定されていないか
		//			Singleton::instance_ and 								// マネージャーが存在するか
		//			Singleton::instance_->version_ == version_ and 			// バージョンが同じか
		//			Singleton::instance_->size_ > index_ and 				// 参照ができる状態か
		//			Singleton::instance_->resources_.at(index_)->mapPtr_;	// データが存在するか
		//	}

		//private:
		//	size_t index_ = (std::numeric_limits<size_t>::max)();
		//	size_t version_ = (std::numeric_limits<size_t>::max)();
		//};

	private:

		class DxResourceItem {
		public:
			DxResourceItem() = default;
			DxResourceItem(std::unique_ptr<DxResourceBuffer<HType>> element) : item_(std::move(element)) {}
			DxResourceItem &operator=(std::unique_ptr<DxResourceBuffer<HType>> element) { item_ = std::move(element); return *this; }

			DxResourceItem(const DxResourceItem &) = delete;
			DxResourceItem &operator=(const DxResourceItem &) = delete;

			DxResourceItem(DxResourceItem &&) = default;
			DxResourceItem &operator=(DxResourceItem &&) = default;

			template<SoLib::IsRealType T>
			DxResourceItem &operator=(T *element) requires(kHasMemory_) {
				std::memcpy(item_->data(), element, sizeof(T)); return *this;
			}
			bool operator==(const DxResourceItem &) const = default;
			bool operator==(const DxResourceBuffer<HType> *that) const { return item_.get() == that; }

			DxResourceBuffer<HType> *const operator->() requires(kHasMemory_) { return item_.get(); }
			const DxResourceBuffer<HType> *const operator->() const requires(kHasMemory_) { return item_.get(); }


			std::unique_ptr<DxResourceBuffer<HType>> item_ = nullptr;

		};

		struct hash {
			size_t operator()(const DxResourceBuffer<HType> *data) const {
				return reinterpret_cast<size_t>(data);
			}

			size_t operator()(const DxResourceItem &data) const {
				return reinterpret_cast<size_t>(data.item_.get());
			}
		};

	public:

		void Init();

		void Clear();

		template <SoLib::IsRealType T>
		DxResourceBuffer<HType> *PushBack(const T &data = {});

		template<typename Itr>
		std::vector<DxResourceBuffer<HType> *> PushBack(Itr begin, Itr end);

	private:

		// リソースのマネージャ
		std::unordered_set<DxResourceItem, hash> resources_;

		// 使っていないリソース [メモリサイズ, データの配列]
		std::map<uint32_t, std::deque<DxResourceItem>> unUsingResource_;

	};

	//	template<D3D12_HEAP_TYPE HType>
	//	inline std::unique_ptr<typename DxResourceBufferPoolManager<HType>::DxResourceBuffer> DxResourceBufferPoolManager<HType>::DxResourceBuffer::Create()
	//	{
	//		std::unique_ptr<DxResourceBuffer> result = std::make_unique<DxResourceBuffer>();
	//		HRESULT hr = S_FALSE;
	//
	//		auto *const device = EngineObject::GetDevice();
	//
	//		// 256バイト単位のアライメント
	//		result->resource_ = CreateBufferResource(device, 0x100);
	//
	//		hr = result->resource_->Map(0, nullptr, reinterpret_cast<void **>(&result->mapPtr_));
	//		assert(SUCCEEDED(hr));
	//
	//#ifdef _DEBUG
	//
	//		result->resource_->SetName(L"DxResourceBufferPoolManager Element");
	//
	//#endif // _DEBUG
	//
	//		return std::move(result);
	//	}


	template<D3D12_HEAP_TYPE HType>
	inline void DxResourceBufferPoolManager<HType>::Init()
	{

	}

	template<D3D12_HEAP_TYPE HType>
	template<SoLib::IsRealType T>
	inline DxResourceBuffer<HType> *DxResourceBufferPoolManager<HType>::PushBack(const T &data)
	{
		// 構築する要素
		DxResourceItem result{};

		// 追加するデータのメモリサイズ
		constexpr uint32_t itemSize = (sizeof(T) + 0xff) >> 8u;

		// 獲得する元の配列
		auto mapItr = unUsingResource_.find(itemSize);

		// 要素が存在したら､それを借りる
		if (mapItr != unUsingResource_.end() and mapItr->second.size() != 0u) {	// 配列が存在して､データが余ってたら実行
			// 保存するデータ
			result = std::move(mapItr->second.front()); // データの先頭を取得する
			mapItr->second.pop_front();	// データの先頭を破棄する
		}
		// 要素が足りなかったら直接生成する
		else {
			// 保存するデータ
			result = std::make_unique<DxResourceBuffer<HType>>(DxResourceBuffer<HType>::Create(sizeof(T)));
		}

		result = &data;

		// そのデータを追加する
		auto itr = resources_.insert(std::move(result));

		// そのデータのポインタを返す
		return itr.first->item_.get();
	}

	template<D3D12_HEAP_TYPE HType>
	template<typename Itr>
	inline std::vector<DxResourceBuffer<HType> *> DxResourceBufferPoolManager<HType>::PushBack(Itr begin, Itr end)
	{
		// イテレータの差分
		const size_t diff = std::distance(begin, end);

		// 構築する要素
		std::vector<DxResourceItem> target{ diff };

		// 使用する型
		using Type = decltype(begin.operator*());

		// 追加するデータのメモリサイズ
		constexpr uint32_t itemSize = (sizeof(decltype(begin.operator*())) + 0xff) >> 8u;

		// 獲得する元の配列
		auto mapItr = unUsingResource_.find(itemSize);

		// すでに完了した数
		size_t successCount = 0u;

		// 要素が存在したら､それを借りる
		if (mapItr != unUsingResource_.end() and mapItr->second.size() != 0u) {	// 配列が存在して､データが余ってたら実行

			// 配列からいくつ取得するか
			const size_t getCount = (std::min)(mapItr->second.size(), diff);
			std::transform(std::make_move_iterator(mapItr->second.rbegin()), std::make_move_iterator(mapItr->second.rbegin() + getCount), target.begin(), [](DxResourceItem &&item) { return std::move(item); });
			mapItr->second.erase(mapItr->second.end(), mapItr->second.end() - getCount);

			// 埋めた数を保存
			successCount = getCount;
		}
		// 要素が足りなかったら直接生成する
		if (successCount != diff) {
			std::for_each(target.begin() + successCount, target.end(), [](DxResourceItem &item) {return std::make_unique<DxResourceBuffer<HType>>(DxResourceBuffer<HType>::Create(itemSize * 0x100)); });
		}

		// データを渡す
		std::transform(begin, end, target.begin(), [](const Type &item) { return &item; });

		std::vector<DxResourceBuffer<HType> *> result{ diff };
		std::transform(target.begin(), target.end(), result.begin(), [](const DxResourceItem &item)->const DxResourceBuffer<HType>*{ return item.item_.get(); });

		// そのデータを追加する
		resources_.insert(std::make_move_iterator(target.begin()), std::make_move_iterator(target.end()));

		// そのデータのポインタを返す
		return result;


	}


}