#pragma once
#include "../../Utils/Containers/Singleton.h"

#include "ResourceObject.h"
#include "../../Utils/SoLib/SoLib_ImGui.h"

namespace SolEngine {

	template <IsResourceObject T, IsResourceSource Source = ResourceSource<T>, SolEngine::IsResourceCreater<T, Source> Creater = ResourceCreater<T, Source>, size_t ContainerSize = 0u>
	class ResourceObjectManager : public SoLib::Singleton<ResourceObjectManager<T, Source, Creater, ContainerSize>> {
		friend SoLib::Singleton<ResourceObjectManager>;
		using Singleton = SoLib::Singleton<ResourceObjectManager<T, Source, Creater, ContainerSize>>;
		ResourceObjectManager() = default;
		ResourceObjectManager(const ResourceObjectManager &) = delete;
		ResourceObjectManager &operator= (const ResourceObjectManager &) = delete;
		~ResourceObjectManager() = default;

	public:

		using DataType = std::unique_ptr<T>;

		struct Handle {

			Handle() = default;
			Handle(const Handle &) = default;
			Handle(Handle &&) = default;
			Handle &operator=(const Handle &) = default;
			Handle &operator=(Handle &&) = default;

			Handle(const Source &source);
			Handle &operator=(const Source &source) { return *this = Handle(source); }

			Handle(const uint32_t handle, const uint32_t version = 0) : handle_(handle), version_(version) {};
			Handle &operator=(const uint32_t handle)
			{
				handle_ = handle;
				return *this;
			}
			bool operator==(const Handle &) const = default;

			const Source *GetSource() const {
				return Singleton::instance_->GetSource(*this);
			}

			// inline operator uint32_t() const { return handle_; }

			uint32_t GetHandle() const { return handle_; }
			uint32_t GetVersion() const { return version_; }

			T *GetResource() { return Singleton::instance_ ? Singleton::instance_->resourceList_.at(handle_).second.second.get() : nullptr; }
			const T *GetResource() const { return Singleton::instance_ ? Singleton::instance_->resourceList_.at(handle_).second.second.get() : nullptr; }

			inline T *operator*() { return GetResource(); }
			inline const T *operator*() const { return GetResource(); }

			inline T *operator->() { return GetResource(); }
			inline const T *operator->() const { return GetResource(); }

			bool IsActive() const { return static_cast<bool>(*this); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const {
				return
					handle_ != (std::numeric_limits<uint32_t>::max)()						// データが最大値(無効値)に設定されていないか
					and Singleton::instance_												// マネージャーが存在するか
					and Singleton::instance_->resourceList_.size() > handle_				// 参照ができる状態か
					and Singleton::instance_->resourceList_.at(handle_).first == version_	// バージョンが一致するか
					and Singleton::instance_->resourceList_.at(handle_).second.second;		// データが存在するか
			}

		private:
			uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
			uint32_t version_ = (std::numeric_limits<uint32_t>::max)();;
		};

	public:

		Handle Load(const Source &source);

		Handle Find(const Source &source) const;

		bool Destory(const Handle &handle);

		const Source *GetSource(const Handle &handle) const;

		/// @brief ImGuiのウィジェットを表示する
		/// @param label 表示するラベル名
		/// @param handle ハンドルID
		/// @return Handle : 選択されたハンドルID 
		Handle ImGuiWidget(const char *const label, const Handle handle) const;

		/// @brief ImGuiのウィジェットを表示する
		/// @param label 表示するラベル名
		/// @return Handle : 選択されたハンドルID 
		Handle ImGuiWidget(const char *const label) const;

	private:
		friend Handle;

		Handle AddData(const Source &source, DataType &&resource);

		std::unordered_map<Source, Handle> findMap_;

		using ItrAndData = std::pair<typename decltype(findMap_)::const_iterator, DataType>;

		std::conditional<ContainerSize == 0u, std::vector<std::pair<uint32_t, ItrAndData>>, std::array<std::pair<uint32_t, ItrAndData>, ContainerSize>>::type resourceList_;

		Creater creater_{};

	};

	template <IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	ResourceObjectManager<T, Source, Creater, ContainerSize>::Handle ResourceObjectManager<T, Source, Creater, ContainerSize>::Load(const Source &source)
	{
		// データを格納する
		Handle result = Find(source);
		// すでにデータが存在する場合はそれを返す
		if (result) { return result; }

		// 引数からリソースを構築
		DataType resource = creater_.CreateObject(source);

		// 有効な値が帰ってきたら保存する
		if (resource) {
			result = AddData(source, std::move(resource));
		}

		return result;
	}

	template <IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	ResourceObjectManager<T, Source, Creater, ContainerSize>::Handle ResourceObjectManager<T, Source, Creater, ContainerSize>::Find(const Source &source) const
	{
		// 検索を行う
		auto itr = findMap_.find(source);
		// 見つかったらそれを返す
		if (itr != findMap_.end()) {

			return itr->second;
		}
		else {
			// 見つからなかったら、不正なデータを返す。
			return Handle{};
		}
	}

	template<IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	inline bool ResourceObjectManager<T, Source, Creater, ContainerSize>::Destory(const Handle &handle)
	{
		// indexから検索
		auto &[version, itrAndData] = resourceList_.at(handle.GetHandle());
		auto &[itr, data] = itrAndData;
		// バージョン検知
		if (version != handle.GetVersion()) { return false; }

		// バージョンが一致していた場合
		// イテレータを破棄
		findMap_.erase(itr);
		itr = findMap_.end();

		// リソースも破棄
		data.reset();

		if constexpr (requires(Creater crafter, uint32_t index) { crafter.Destroy(index); }) {
			creater_.Destroy(handle.GetHandle());
		}

		// バージョンを1つ上げる
		version++;

		return true;
	}

	template<IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	inline const Source *ResourceObjectManager<T, Source, Creater, ContainerSize>::GetSource(const Handle &handle) const
	{
		// indexから検索
		auto &[version, itrAndData] = resourceList_.at(handle.GetHandle());
		// バージョン検知
		if (version != handle.GetVersion()) { return nullptr; }
		auto &[itr, data] = itrAndData;

		return &itr->first;
	}

	template<IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	inline ResourceObjectManager<T, Source, Creater, ContainerSize>::Handle ResourceObjectManager<T, Source, Creater, ContainerSize>::AddData(const Source &source, DataType &&resource)
	{

		typename decltype(resourceList_)::iterator itr;
		if (resourceList_.size() == findMap_.size()) {
			itr = resourceList_.end();
		}
		else {
			itr = std::find_if(resourceList_.begin(), resourceList_.end(), [](const auto &item) ->bool {return item.second.second == nullptr; });
		}

		uint32_t index = 0;
		uint32_t version = 0;

		if (itr != resourceList_.end()) {
			index = static_cast<uint32_t>(std::distance(resourceList_.begin(), itr));
		}
		else {
			if constexpr (ContainerSize == 0) {
				index = static_cast<uint32_t>(resourceList_.size());
				resourceList_.push_back({});
				itr = std::prev(resourceList_.end());
			}
			else {
				assert("保存領域を超えました。");
				return 0;
			}
		}
		version = itr->first;


		// 検索用に保存
		findMap_.insert({ source, {index, version} });

		// 構築したデータを格納
		*itr = { 0, std::make_pair(findMap_.find(source), std::move(resource)) };

		return index;
	}

	template<IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	inline ResourceObjectManager<T, Source, Creater, ContainerSize>::Handle ResourceObjectManager<T, Source, Creater, ContainerSize>::ImGuiWidget(const char *const label, const Handle handle) const
	{


		uint32_t result = SoLib::ImGuiWidget(label, &resourceList_, handle.GetHandle(),
			[this](uint32_t index)->std::string
			{
				if (resourceList_.size() <= index) { return ""; }
				const auto &itrAndData = resourceList_.at(index).second;
				return itrAndData.second ? itrAndData.first->first.ToStr() : "";

				//auto findItr = std::find_if(findMap_.begin(), findMap_.end(),
				//	[this, index](auto itr) { return itr.second.GetHandle() == index; });
				//return findItr != findMap_.end() and findItr->second ? findItr->first.ToStr() : "";
			}
		);

		return Handle{ result };
	}
	template<IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	inline ResourceObjectManager<T, Source, Creater, ContainerSize>::Handle ResourceObjectManager<T, Source, Creater, ContainerSize>::ImGuiWidget(const char *const label) const
	{
		static Handle result;

		result = ImGuiWidget(label, result);

		return result;
	}
	template<IsResourceObject T, IsResourceSource Source, SolEngine::IsResourceCreater<T, Source> Creater, size_t ContainerSize>
	inline ResourceObjectManager<T, Source, Creater, ContainerSize>::Handle::Handle(const Source &source)
	{
		Singleton::instance_ ? *this = Singleton::instance_->Load(source) : Handle{};
	}
}

//namespace std {
//	template<>
//	struct hash<SolEngine::ResourceObjectManager<SolEngine::ModelData>::Handle> {
//		size_t operator()(const SolEngine::ResourceObjectManager<SolEngine::ModelData>::Handle data) const {
//			size_t result{};
//
//			result = data.GetVersion();
//			result <<= 32;
//			result += data.GetHandle();
//
//			return result;
//		}
//	};
//}