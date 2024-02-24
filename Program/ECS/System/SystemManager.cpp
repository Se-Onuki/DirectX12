#include "SystemManager.h"

ECS::ISystem *ECS::SystemManager::AddSystem(std::unique_ptr<ECS::ISystem> system)
{

	//auto systemGroup = system->GetSimlationSystemGroup();
	// Worldに変更をもたらさないなら0にしておく
	uint32_t isConstTarget = static_cast<uint32_t>(not system->IsConstSystem());

	return systemList_.at(isConstTarget).emplace_back(std::move(system)).get();
}

void ECS::SystemManager::clear()
{
	//for (auto &systemPair : systemList_) {
	for (auto &systems : systemList_) {
		systems.clear();
	}
	//}
}

void ECS::SystemManager::Update(::World *world, const float deltaTime)
{

	//for (auto &systemPair : systemList_) {
	for (auto &systems : systemList_) {
		for (auto &system : systems) {
			system->OnUpdate(world, deltaTime);
		}
	}
	//}
}
