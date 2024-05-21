#pragma once
#include "../../Utils/Containers/Singleton.h"

#include "ResourceObject.h"
#include "../DirectBase/Base/Shader.h"

namespace SolEngine {

	template <IsResourceObject T, SoLib::IsRealType CreateSource = IResourceSource<T>>
	class ResourceObjectManager : public SoLib::Singleton<ResourceObjectManager<T, CreateSource>> {
		friend SoLib::Singleton<ResourceObjectManager>;
		using Singleton = SoLib::Singleton<ResourceObjectManager<T, CreateSource>>;
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

			Shader *GetShader() { return Singleton::instance_ ? Singleton::instance_->resources_.at(handle_).get() : nullptr; }
			const Shader *GetShader() const { return Singleton::instance_ ? Singleton::instance_->resources_.at(handle_).get() : nullptr; }

			inline Shader *operator*() { return GetShader(); }
			inline const Shader *operator*() const { return GetShader(); }

			// inline operator Shader &() const { return *instance_->resources_.at(handle_); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const {
				return
					handle_ != (std::numeric_limits<uint32_t>::max)()	// データが最大値(無効値)に設定されていないか
					and Singleton::instance_										// マネージャーが存在するか
					and handle_ < Singleton::instance_->resources_.size()			// 参照ができる状態か
					and Singleton::instance_->resources_.at(handle_);			// データが存在するか
			}

		private:
			uint32_t handle_ = (std::numeric_limits<uint32_t>::max)();
			//inline static ResourceObjectManager *const manager_ = ResourceObjectManager::GetInstance();
		};

		Handle Load(const CreateSource &cleate_source);

		Handle Find(const CreateSource &cleate_source);

	private:
		friend Handle;

		std::vector<std::unique_ptr<Shader>> resources_;
		std::unordered_map<CreateSource, Handle> findMap_;


	};

	template <IsResourceObject T, SoLib::IsRealType CreateSource>
	ResourceObjectManager<T, CreateSource>::Handle ResourceObjectManager<T, CreateSource>::Load(const CreateSource &createSource)
	{
		// データを格納する
		Handle result = Find(createSource);
		// すでにデータが存在する場合はそれを返す
		if (result) { return result; }

		// 引数からシェーダを構築
		std::unique_ptr<Shader> shader = createSource.CreateObject();

		// 構築したデータを格納
		resources_.push_back(std::move(shader));

		// 添え字を代入
		result = static_cast<uint32_t>(resources_.size() - 1);

		// 検索用に保存
		findMap_.insert({ createSource, result });

		return result;
	}

	template <IsResourceObject T, SoLib::IsRealType CreateSource>
	ResourceObjectManager<T, CreateSource>::Handle ResourceObjectManager<T, CreateSource>::Find(const CreateSource &createSource)
	{
		// 検索を行う
		auto itr = findMap_.find(createSource);
		// 見つかったらそれを返す
		if (itr != findMap_.end()) {

			return itr->second;
		}
		else {
			// 見つからなかったら、不正なデータを返す。
			return Handle{ (std::numeric_limits<uint32_t>::max)() };
		}
	}
}
