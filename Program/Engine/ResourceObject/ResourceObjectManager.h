#pragma once
#include "../../Utils/Containers/Singleton.h"

#include "ResourceObject.h"
#include "../../Utils/SoLib/SoLib_ImGui.h"

namespace SolEngine {

	template <IsResourceObject T, SoLib::IsRealType Source = ResourceSource<T>, SoLib::IsRealType Creater = ResourceCreater<T>>
	class ResourceObjectManager : public SoLib::Singleton<ResourceObjectManager<T, Source, Creater>> {
		friend SoLib::Singleton<ResourceObjectManager>;
		using Singleton = SoLib::Singleton<ResourceObjectManager<T, Source, Creater>>;
		ResourceObjectManager() = default;
		ResourceObjectManager(const ResourceObjectManager &) = delete;
		ResourceObjectManager &operator= (const ResourceObjectManager &) = delete;
		~ResourceObjectManager() = default;

	public:

		//using CreateSource = T::CreateSource;
		//using CreateSource = ShaderCreater;


		struct Handle {

			Handle() = default;
			Handle(const Handle &) = default;
			Handle(Handle &&) = default;
			Handle &operator=(const Handle &) = default;
			Handle &operator=(Handle &&) = default;

			Handle(const uint32_t handle) : handle_(handle) {};
			Handle &operator=(const uint32_t handle)
			{
				handle_ = handle;
				return *this;
			}

			// inline operator uint32_t() const { return handle_; }

			uint32_t GetHandle() const { return handle_; }

			T *GetResource() { return Singleton::instance_ ? Singleton::instance_->resourceList_.at(handle_).second.get() : nullptr; }
			const T *GetResource() const { return Singleton::instance_ ? Singleton::instance_->resourceList_.at(handle_).second.get() : nullptr; }

			inline T *operator*() { return GetResource(); }
			inline const T *operator*() const { return GetResource(); }

			// inline operator Shader &() const { return *instance_->resources_.at(handle_); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const {
				return
					handle_ != (std::numeric_limits<uint32_t>::max)()		// データが最大値(無効値)に設定されていないか
					and Singleton::instance_								// マネージャーが存在するか
					and handle_ < Singleton::instance_->resourceList_.size()	// 参照ができる状態か
					and Singleton::instance_->resourceList_.at(handle_).second;		// データが存在するか
			}

		private:
			uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
			//inline static ResourceObjectManager *const manager_ = ResourceObjectManager::GetInstance();
		};

		Handle Load(const Source &source);

		Handle Find(const Source &source);

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

		Handle AddData(const Source &source, std::unique_ptr<T> resource);

		std::unordered_map<Source, Handle> findMap_;
		std::vector<std::pair<typename const decltype(findMap_)::const_iterator, std::unique_ptr<T>>> resourceList_;

		Creater creater_;

	};

	template <IsResourceObject T, SoLib::IsRealType Source, SoLib::IsRealType Creater>
	ResourceObjectManager<T, Source, Creater>::Handle ResourceObjectManager<T, Source, Creater>::Load(const Source &source)
	{
		// データを格納する
		Handle result = Find(source);
		// すでにデータが存在する場合はそれを返す
		if (result) { return result; }

		// 引数からリソースを構築
		std::unique_ptr<T> resource = creater_.CreateObject(source);

		result = AddData(source, std::move(resource));

		return result;
	}

	template <IsResourceObject T, SoLib::IsRealType Source, SoLib::IsRealType Creater>
	ResourceObjectManager<T, Source, Creater>::Handle ResourceObjectManager<T, Source, Creater>::Find(const Source &source)
	{
		// 検索を行う
		auto itr = findMap_.find(source);
		// 見つかったらそれを返す
		if (itr != findMap_.end()) {

			return itr->second;
		}
		else {
			// 見つからなかったら、不正なデータを返す。
			return Handle{ (std::numeric_limits<uint32_t>::max)() };
		}
	}

	template<IsResourceObject T, SoLib::IsRealType Source, SoLib::IsRealType Creater>
	inline ResourceObjectManager<T, Source, Creater>::Handle ResourceObjectManager<T, Source, Creater>::AddData(const Source &source, std::unique_ptr<T> resource)
	{

		// 添え字を代入
		uint32_t result = static_cast<uint32_t>(resourceList_.size());

		// 検索用に保存
		findMap_.insert({ source, result });

		// 構築したデータを格納
		resourceList_.push_back({ findMap_.find(source), std::move(resource) });

		return result;
	}

	template<IsResourceObject T, SoLib::IsRealType Source, SoLib::IsRealType Creater>
	inline ResourceObjectManager<T, Source, Creater>::Handle ResourceObjectManager<T, Source, Creater>::ImGuiWidget(const char *const label, const Handle handle) const
	{


		uint32_t result = SoLib::ImGuiWidget(label, &resourceList_, handle.GetHandle(),
			[this](uint32_t index)->std::string
			{
				return Handle{ index } ? resourceList_.at(index).first->first.ToStr() : "";

				//auto findItr = std::find_if(findMap_.begin(), findMap_.end(),
				//	[this, index](auto itr) { return itr.second.GetHandle() == index; });
				//return findItr != findMap_.end() and findItr->second ? findItr->first.ToStr() : "";
			}
		);

		return Handle{ result };
	}
	template<IsResourceObject T, SoLib::IsRealType Source, SoLib::IsRealType Creater>
	inline ResourceObjectManager<T, Source, Creater>::Handle ResourceObjectManager<T, Source, Creater>::ImGuiWidget(const char *const label) const
	{
		static Handle result;

		result = ImGuiWidget(label, result);

		return result;
	}
}
