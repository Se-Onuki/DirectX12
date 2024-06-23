#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <memory>
#include <mutex>

namespace SoLib {

	template <SoLib::IsRealType T>
	class Singleton {
		Singleton(const Singleton &) = delete;
		Singleton &operator=(const Singleton &) = delete;

		struct SingletonDeleter {
			void operator()(T *ptr) const {
				delete ptr;
			}
		};

	protected:
		Singleton() = default;
		~Singleton() = default;

	public:

		static T *const GetInstance();

		static void Finalize();

	protected:

		static std::unique_ptr<T, SingletonDeleter> instance_;
		static std::mutex mutex_;

	};

	template<SoLib::IsRealType T>
	std::unique_ptr<T, typename Singleton<T>::SingletonDeleter> Singleton<T>::instance_ = nullptr;

	template<SoLib::IsRealType T>
	std::mutex Singleton<T>::mutex_;

	template<SoLib::IsRealType T>
	inline T *const Singleton<T>::GetInstance() {
		{
			// マルチスレッドのロックを掛ける
			std::lock_guard<std::mutex> lock(mutex_);
			// 実体がない場合
			if (not instance_) {
				// 生成
				instance_.reset(new T);
			}
		}
		// ポインタを返す
		return instance_.get();
	}

	template<SoLib::IsRealType T>
	inline void Singleton<T>::Finalize() {
		instance_.reset();
	}

}
