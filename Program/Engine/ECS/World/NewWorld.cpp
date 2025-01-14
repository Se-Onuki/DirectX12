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

		for (uint32_t i : prefab.GetArchetype().required_.GetIndexList()) {
			if (not prefab.GetArchetype().required_.Get()[i]) { continue; }
			const auto &typeKey = compRegistry->typeDatas_[i];

			auto ptr = entity.chunk_->GetComp(i, entity.totalIndex_);
			std::memcpy(ptr, compItr.at(typeKey.typeIndex_).get(), typeKey.typeSize_);
		}

		return entity;

	}

	EntityList<false> World::CreateEntity(const Archetype &archetype, uint32_t count)
	{
		auto chunk = CreateOrGetChunk(archetype);

		auto index = chunk->emplace_back(count);

		EntityList<false> result{ chunk, index.first, index.second };
		return result;

	}

	EntityList<false> World::CreateEntity(const Prefab &prefab, uint32_t count)
	{
		auto &&entityList = CreateEntity(prefab.GetArchetype(), count);
		const auto *compRegistry = ECS::ComponentRegistry::GetInstance();
		const auto &prefabComp = prefab.GetComponentMap();

		for (uint32_t i : prefab.GetArchetype().required_.GetIndexList()) {
			if (not prefab.GetArchetype().required_.Get()[i]) { continue; }
			const auto &typeKey = compRegistry->typeDatas_[i];

			auto *const src = prefabComp.at(typeKey.typeIndex_).get();
			auto component = entityList.GetChunk()->GetComponent(i);

			for (const auto entity : entityList.ItrRange()) {
				auto ptr = &component->at(entity);
				std::memcpy(ptr, src, typeKey.typeSize_);
			}
		}

		return entityList;
	}

}
