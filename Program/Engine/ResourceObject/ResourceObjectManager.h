#pragma once
#include "../../Utils/Containers/Singleton.h"

#include "ResourceObject.h"
#include "../DirectBase/Base/Shader.h"

namespace SolEngine {

	//template <IsResourceObject T>
	class ResourceObjectManager : public SoLib::Singleton<ResourceObjectManager> {
		friend SoLib::Singleton<ResourceObjectManager>;
		ResourceObjectManager() = default;
		ResourceObjectManager(const ResourceObjectManager &) = delete;
		ResourceObjectManager &operator= (const ResourceObjectManager &) = delete;
		~ResourceObjectManager() = default;

	public:

		//using CreateSource = T::CreateSource;
		using CreateSource = ShaderName;


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

			Shader *GetShader() { return instance_ ? instance_->resources_.at(handle_).get() : nullptr; }
			const Shader *GetShader() const { return instance_ ? instance_->resources_.at(handle_).get() : nullptr; }

			inline Shader &operator*() { return *instance_->resources_.at(handle_); }
			inline const Shader &operator*() const { return *instance_->resources_.at(handle_); }

			// inline operator Shader &() const { return *instance_->resources_.at(handle_); }

			/// @brief このデータが有効であるか
			explicit inline operator bool() const {
				return
					handle_ != (std::numeric_limits<uint32_t>::max)()	// データが最大値(無効値)に設定されていないか
					and instance_										// マネージャーが存在するか
					and handle_ < instance_->resources_.size()			// 参照ができる状態か
					and instance_->resources_.at(handle_);			// データが存在するか
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


	/*template <IsResourceObject T>
	ResourceObjectManager<T> *const ResourceObjectManager<T>::Handle::manager_ = ResourceObjectManager<T>::GetInstance();
*/

}
