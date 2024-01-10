#include "SystemManager.h"

void ECS::SystemManager::AddSystem(std::unique_ptr<ECS::ISystem> system) {
	systemList_.push_back(std::move(system));
}

void ECS::SystemManager::clear() {
	systemList_.clear();
}

void ECS::SystemManager::Update(const float deltaTime) {

	for (auto &system : systemList_) {
		system->OnUpdate(deltaTime);
	}
}
