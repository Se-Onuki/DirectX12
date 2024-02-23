#include "SystemManager.h"

void ECS::SystemManager::AddSystem(std::unique_ptr<ECS::ISystem> system)
{

    auto systemGroup = system->GetSimlationSystemGroup();
	// Worldに変更をもたらさないなら0にしておく
    uint32_t isConstTarget = static_cast<uint32_t>(not system->IsConstSystem());

    systemList_.at(static_cast<uint32_t>(systemGroup)).at(isConstTarget).emplace_back(std::move(system));
}

void ECS::SystemManager::clear()
{
    for (auto &systemPair : systemList_) {
        for (auto &systems : systemPair) {
            systems.clear();
        }
    }
}

void ECS::SystemManager::Update(const float deltaTime)
{

    for (auto &systemPair : systemList_) {
        for (auto &systems : systemPair) {
            for (auto &system : systems) {
                system->OnUpdate(deltaTime);
            }
        }
    }
}
