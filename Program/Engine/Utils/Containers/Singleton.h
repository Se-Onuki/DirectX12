#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <memory>
#include <mutex>
#include <typeindex>

/// シングルトンの記述のマクロ
#define ConstructSingleton(Name) Name() = default;\
Name(const Name &) = delete;\
Name &operator= (const Name &) = delete;\
~Name() = default;\
friend SoLib::Singleton<Name>;

namespace SoLib {
	/// @brief シングルトンの基底クラス
	class ISingleton {
	public:
		virtual void FinalizeSelf() {};
	};
	/// @brief シングルトンのスタック
	class SingletonStacker;
	/// @brief CRTPによるシングルトンクラス
	/// @tparam T 定義するクラス
	template <SoLib::IsRealType T>
	class Singleton : public ISingleton {
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

		void FinalizeSelf() override { Singleton::Finalize(); }

	protected:
		// インスタンスのポインタ
		static std::unique_ptr<T, SingletonDeleter> instance_;
		// 一度だけ実行されるフラグ
		static std::once_flag onceFlag_;

	private:

		// 生成関数
		static void Generate();

	};

	/// @brief シングルトンのスタック
	class SingletonStacker : public Singleton<SingletonStacker> {
		ConstructSingleton(SingletonStacker)
	public:
		// インスタンスの配列
		std::list<std::unique_ptr<SoLib::ISingleton>> instanceList_;
	};

	template<SoLib::IsRealType T>
	std::unique_ptr<T, typename Singleton<T>::SingletonDeleter> Singleton<T>::instance_ = nullptr;

	template<SoLib::IsRealType T>
	std::once_flag Singleton<T>::onceFlag_;

	template<SoLib::IsRealType T>
	inline T *const Singleton<T>::GetInstance() {
		// https://af-e.net/cpp-singleton-thread-safe

		// 一度だけ生成関数を呼び出す
		std::call_once(onceFlag_, Generate);
		// ポインタを返す
		return instance_.get();
	}

	template<SoLib::IsRealType T>
	inline void Singleton<T>::Finalize() {
		instance_.reset();
	}

	template<SoLib::IsRealType T>
	inline void Singleton<T>::Generate()
	{
		instance_.reset(new T);
	}

}
