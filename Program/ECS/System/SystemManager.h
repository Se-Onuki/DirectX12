#pragma once
#include "ISystem.h"
#include <list>
#include <memory>
#include <array>

namespace ECS {

	class SystemManager
	{
	public:
		SystemManager() = default;
		~SystemManager() = default;

		void AddSystem(std::unique_ptr<ECS::ISystem> system);

		template <typename T>
		void AddSystem();

		void clear();

		void Update(const float deltaTime);

	private:

		std::array<std::array<std::list<std::unique_ptr<ECS::ISystem>>, 2u>, static_cast<uint32_t>(SimulationSystemGroup::kMaxSize)> systemList_;


	};

	template<typename T>
	inline void SystemManager::AddSystem() {
		AddSystem(std::make_unique<T>());
	}
}