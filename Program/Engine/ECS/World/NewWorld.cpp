/// @file NewWorld.cpp
/// @brief ECSのワールド
/// @author ONUKI seiya
#include "NewWorld.h"

namespace ECS {

	const EntityClass &World::CreateEntity(const Archetype &archetype)
	{
		auto chunk = CreateOrGetChunk(archetype);

		uint32_t index = chunk->emplace_back();

		return chunk->GetEntity(index);
	}

	const EntityClass &World::CreateEntity(const Prefab &prefab)
	{
		auto &&entity = CreateEntity(prefab.GetArchetype());

		const auto *compRegistry = ECS::ComponentRegistry::GetInstance();
		const auto &compItr = prefab.GetComponentMap();

		for (uint32_t i : prefab.GetArchetype().compFlag_.GetIndexList()) {
			if (not prefab.GetArchetype().compFlag_.Get()[i]) { continue; }
			const auto &typeKey = compRegistry->typeDatas_[i];

			auto ptr = entity.chunk_->GetComp(i, entity.totalIndex_);
			std::memcpy(ptr, compItr.at(typeKey.typeIndex_).get(), typeKey.typeSize_);
		}

		return entity;

	}

	std::vector<EntityClass> World::CreateEntity(const Archetype &archetype, uint32_t count)
	{
		auto chunk = CreateOrGetChunk(archetype);

		auto index = chunk->emplace_back(count);

		std::vector<EntityClass> result{ index.second - index.first };
		for (uint32_t i = 0; i < result.size(); i++) {
			result[i] = chunk->GetEntity(index.first + i);
		}
		return result;

	}

	std::vector<EntityClass> World::CreateEntity(const Prefab &prefab, uint32_t count)
	{
		auto &&entityList = CreateEntity(prefab.GetArchetype(), count);
		const auto *compRegistry = ECS::ComponentRegistry::GetInstance();
		const auto &compItr = prefab.GetComponentMap();

		for (uint32_t i : prefab.GetArchetype().compFlag_.GetIndexList()) {
			if (not prefab.GetArchetype().compFlag_.Get()[i]) { continue; }
			const auto &typeKey = compRegistry->typeDatas_[i];

			auto *const src = compItr.at(typeKey.typeIndex_).get();
			for (const auto &entity : entityList) {
				auto ptr = entity.chunk_->GetComp(i, entity.totalIndex_);
				std::memcpy(ptr, src, typeKey.typeSize_);
			}
		}

		return entityList;
	}

}
