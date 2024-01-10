#pragma once
#include "ISystem.h"
#include <list>
#include <memory>

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
		std::list<std::unique_ptr<ECS::ISystem>> systemList_;

	};

	template<typename T>
	inline void SystemManager::AddSystem() {
		AddSystem(std::make_unique<T>());
	}
}