#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <memory>

namespace SoLib {

	template <SoLib::IsNotPointer T>
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

	};

	template<SoLib::IsNotPointer T>
	std::unique_ptr<T, typename Singleton<T>::SingletonDeleter> Singleton<T>::instance_ = nullptr;

	template<SoLib::IsNotPointer T>
	inline T *const Singleton<T>::GetInstance() {
		// 実体がない場合
		if (not instance_) {
			// 生成
			instance_.reset(new T);
		}
		// ポインタを返す
		return instance_.get();
	}

	template<SoLib::IsNotPointer T>
	inline void Singleton<T>::Finalize() {
		instance_.reset();
	}

}
