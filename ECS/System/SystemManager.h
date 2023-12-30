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

	private:
		std::list<std::unique_ptr<ECS::ISystem>> systemList_;

	};
}