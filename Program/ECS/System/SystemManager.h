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

		template <SoLib::IsBased<ECS::ISystem> T>
		T *AddSystem();

		void clear();

		void Update(::World *world, const float deltaTime);

	private:

		std::array<std::list<std::unique_ptr<ECS::ISystem>>, 2u> systemList_;


	};

	template<SoLib::IsBased<ECS::ISystem> T>
	inline T *SystemManager::AddSystem() {
		return static_cast<T*>(AddSystem(std::make_unique<T>()));
	}
}