#pragma once
#include "ISystem.h"
#include <list>
#include <memory>
#include <array>
#include "../../Utils/SoLib/SoLib_Traits.h"

namespace ECS {

	class SystemManager
	{
	public:
		SystemManager() = default;
		~SystemManager() = default;

		ECS::ISystem *AddSystem(std::unique_ptr<ECS::ISystem> system);

		/// @brief システムの追加
		/// @tparam T 追加する型
		/// @param ...args 引数
		/// @return 追加されたシステム
		template <SoLib::IsBased<ECS::ISystem> T, typename... Args>
			requires (std::is_invocable_r<T, decltype(&T::Create), Args...>::value)
		T *AddSystem(Args&&... args);

		template <SoLib::IsBased<ECS::ISystem> T>
		T *AddSystem();

		template <SoLib::IsBased<ECS::ISystem> T>
		T *AddSystem(const T &);

		template <SoLib::IsBased<ECS::ISystem> T>
		T *AddSystem(T &&);

		void clear();

		void Update(::World *world, const float deltaTime);

	private:

		std::array<std::list<std::unique_ptr<ECS::ISystem>>, 2u> systemList_;


	};

	template<SoLib::IsBased<ECS::ISystem> T, typename... Args>
		requires (std::is_invocable_r<T, decltype(&T::Create), Args...>::value)
	inline T *SystemManager::AddSystem(Args&&... args) {
		return static_cast<T *>(AddSystem(std::make_unique<T>(T::Create(std::forward<Args>(args)...))));
	}

	template<SoLib::IsBased<ECS::ISystem> T>
	inline T *SystemManager::AddSystem() {
		return static_cast<T *>(AddSystem(std::make_unique<T>()));
	}
	template<SoLib::IsBased<ECS::ISystem> T>
	inline T *SystemManager::AddSystem(const T &right) {
		return static_cast<T *>(AddSystem(std::make_unique<T>(right)));
	}
	template<SoLib::IsBased<ECS::ISystem> T>
	inline T *SystemManager::AddSystem(T &&right) {
		return static_cast<T *>(AddSystem(std::make_unique<T>(std::move(right))));
	}
}