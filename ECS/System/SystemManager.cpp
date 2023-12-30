#include "SystemManager.h"

void ECS::SystemManager::AddSystem(std::unique_ptr<ECS::ISystem> system) {
	systemList_.push_back(std::move(system));
}
