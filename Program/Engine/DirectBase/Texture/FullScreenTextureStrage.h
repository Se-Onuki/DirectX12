#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

#include "../Base/DirectXCommon.h"
#include "../Base/EngineObject.h"

namespace SolEngine {

	class FullScreenTexture : public SolEngine::EngineObject {
	public:

		static std::unique_ptr<FullScreenTexture> Create(const DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>::Handle &rtvHandle, const DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::Handle &srvHandle);

	private:

		// クリア時の色
		SoLib::Color::RGB4 clearColor_ = 0xFF0000FF; // 赤を指定しておく

		ComPtr<ID3D12Resource> renderTargetTexture_;

		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>::Handle rtvHandle_;
		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::Handle srvHandle_;
	};


	class FullScreenTextureStrage : public SoLib::Singleton<FullScreenTextureStrage>, public EngineObject {
		friend SoLib::Singleton<FullScreenTextureStrage>;
		using Singleton = SoLib::Singleton<FullScreenTextureStrage>;
		FullScreenTextureStrage() = default;
		FullScreenTextureStrage(const FullScreenTextureStrage &) = delete;
		FullScreenTextureStrage &operator= (const FullScreenTextureStrage &) = delete;
		~FullScreenTextureStrage() = default;

	public:

		using DataType = FullScreenTexture;
		inline static constexpr uint32_t kContainerSize_ = 8u;

		struct Handle {

			Handle() = default;
			Handle(const Handle &) = default;
			Handle(Handle &&) = default;
			Handle &operator=(const Handle &) = default;
			Handle &operator=(Handle &&) = default;

			Handle(const uint32_t handle, const uint32_t version = 0) : handle_(handle), version_(version) {};
			Handle &operator=(const uint32_t handle)
			{
				handle_ = handle;
				return *this;
			}
			bool operator==(const Handle &) const = default;

			// inline operator uint32_t() const { return handle_; }

			uint32_t GetHandle() const { return handle_; }
			uint32_t GetVersion() const { return version_; }

			DataType *GetResource() { return Singleton::instance_ ? Singleton::instance_->resourceList_.at(handle_).second.second.get() : nullptr; }
			const DataType *GetResource() const { return Singleton::instance_ ? Singleton::instance_->resourceList_.at(handle_).second.second.get() : nullptr; }

			inline DataType *operator*() { return GetResource(); }
			inline const DataType *operator*() const { return GetResource(); }

			inline DataType *operator->() { return GetResource(); }
			inline const DataType *operator->() const { return GetResource(); }

			bool IsActive() const { return static_cast<bool>(*this); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const {
				return
					handle_ != (std::numeric_limits<uint32_t>::max)()						// データが最大値(無効値)に設定されていないか
					and Singleton::instance_												// マネージャーが存在するか
					and Singleton::instance_->resourceList_.size() > handle_				// 参照ができる状態か
					and Singleton::instance_->resourceList_.at(handle_).first == version_	// バージョンが一致するか
					and Singleton::instance_->resourceList_.at(handle_).second.first;		// データが存在するか
			}

		private:
			uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
			uint32_t version_ = (std::numeric_limits<uint32_t>::max)();;
		};

	public:

		void Init();

		Handle Allocate();

		bool Destory(const Handle handle);

	private:
		friend Handle;

		std::array<std::pair<uint32_t, std::pair<bool, std::unique_ptr<DataType>>>, kContainerSize_> resourceList_;

		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> rtvDescHeap_{ EngineObject::GetDevice(), kContainerSize_, false };
		DescHeapCbvSrvUav::HeapRange srvHeapRange_;

	};

	inline void FullScreenTextureStrage::Init()
	{
		srvHeapRange_ = GetDescHeapCbvSrvUav()->RequestHeapAllocation(kContainerSize_);
		for (uint32_t i = 0; i < kContainerSize_; i++) {
			auto &[version, flagAndItem] = resourceList_[i];
			auto &[flag, item] = flagAndItem;
			item = DataType::Create(
				rtvDescHeap_.GetHandle(0, i), srvHeapRange_.GetHandle(i));

		}
	}

	inline FullScreenTextureStrage::Handle FullScreenTextureStrage::Allocate()
	{

		typename decltype(resourceList_)::iterator itr;

		// falseになっている最初のデータを検索
		itr = std::find_if(resourceList_.begin(), resourceList_.end(), [](const auto &item) ->bool {return item.second.first == false; });

		uint32_t index = 0;
		uint32_t version = 0;

		if (itr != resourceList_.end()) {
			index = static_cast<uint32_t>(std::distance(resourceList_.begin(), itr));
		}
		else {
			return Handle{};
		}

		// バージョンを検出
		version = itr->first;

		// 構築したデータを格納
		itr->second.first = true;

		return Handle{ index, version };
	}

	inline bool FullScreenTextureStrage::Destory(const Handle handle)
	{
		// indexから検索
		auto &[version, flagAndData] = resourceList_.at(handle.GetHandle());
		auto &[flag, data] = flagAndData;
		// バージョン検知
		if (version != handle.GetVersion()) { return false; }

		// バージョンを1つ上げる
		version++;

		// flagを折る
		flag = false;

		return true;
	}
}