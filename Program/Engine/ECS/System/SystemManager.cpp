#include "SystemManager.h"

ECS::ISystem *ECS::SystemManager::AddSystem(std::unique_ptr<ECS::ISystem> system)
{
	return systemList_.emplace_back(std::move(system)).get();
}

void ECS::SystemManager::clear()
{
	systemList_.clear();
}

void ECS::SystemManager::Update(::World *world, const float deltaTime)
{
	for (auto &system : systemList_) {
		system->OnUpdate(world, deltaTime);
	}
}
