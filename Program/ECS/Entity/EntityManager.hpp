#pragma once

#include <typeinfo>

#include "../Chunk/Chunk.hpp"
#include "../Component/Component.hpp"
#include "../Entity/Entity.hpp"
#include "../World/World.hpp"
#include "Prefab.h"

namespace ECS {

	class EntityManager {
	public:
		EntityManager(World *world);

		/*template<typename... Ts>
		Entity CreateEntity(const Ts&... value) {
			Entity entity{};
			Archetype archetype;
			archetype.AddClassData<Ts>();

			entity.chunkId_ = world_->FindMatchChunk(archetype);
			if (entity.chunkId_ == UINT32_MAX) {
				entity.chunkId_ = world_->CreateChunk(archetype);
			}

			Chunk* chunk = world_->GetChunk(entity.chunkId_);
			chunk->entityCount_++;
			(chunk->Get<Ts>())
			return entity;
		}*/

		template<typename... Ts>
		const std::list<ECS::Entity> CreateEntity(uint32_t count = 1u) {
			if (not count) {
				return {};
			}
			Archetype archetype;
			archetype.AddClassData<Ts...>();

			return CreateEntity(archetype, count);
		}

		//template<typename... Ts>
		const std::list<ECS::Entity> CreateEntity(const ECS::Prefab &prefab, uint32_t count = 1u) {
			std::list<ECS::Entity> entityList{};
			if (not count) {
				return entityList;
			}

			auto mArray = world_->GetChunk(prefab.GetArchetype());

			if (not mArray) {
				mArray = world_->CreateChunk(prefab.GetArchetype());
			}
			for (uint32_t i = 0u; i < count; i++) {
				ECS::Entity &entity = entityList.emplace_back();

				entity.arrayPtr_ = mArray;
				entity.totalIndex_ = mArray->push_back(prefab);
			}

			return entityList;
		}

		const std::list<ECS::Entity> CreateEntity(const Archetype &archetype, uint32_t count = 1u) {
			std::list<ECS::Entity> entityList{};
			if (not count) {
				return entityList;
			}

			auto mArray = world_->GetChunk(archetype);

			if (not mArray) {
				mArray = world_->CreateChunk(archetype);
			}
			for (uint32_t i = 0u; i < count; i++) {
				ECS::Entity &entity = entityList.emplace_back();

				entity.arrayPtr_ = mArray;
				entity.totalIndex_ = mArray->push_back();
			}

			return entityList;
		}

		/*

		ECS::Entity CreateEntity(const uint32_t &chunkId) {
			Chunk *targetChunk = world_->GetChunk(chunkId);
			targetChunk->entityCount_++;
			if (targetChunk->maxCount_ < targetChunk->entityCount_) {
			}
			ECS::Entity{ chunkId, targetChunk->entityCount_ };
		}*/

		// std::vector<Chunk&> FindHitChunk(const Archetype& archetype);

		/*template<typename T> T &GetComponent(Entity entity) {
			return world_->GetChunk(entity.chunkId_)->GetArray<T>()[entity.chunkIndex_];
		}*/

		template<typename T, typename... Ts>
		std::tuple<T *const, Ts *const...> GetComponent(const ECS::Entity &entity) {

			return entity.arrayPtr_->GetItem<T, Ts...>(entity.totalIndex_);
		}

		template<typename T>
		T &GetOneComponent(const ECS::Entity &entity) {

			return entity.arrayPtr_->GetItem<T>(entity.totalIndex_);
		}

		//template<typename T> void SetComponent(Entity entity, T &component) {
		//	Chunk *chunk = world_->GetChunk(entity.chunkId_);
		//	CustomArray &componentArray = chunk->GetCustomArray<T>();
		//	if (componentArray.size() == chunk->size() &&
		//		componentArray.size() <=
		//		entity.chunkIndex_) // 配列の要素数がchunkの制御番号を超えていない +
		//		// エンティティの登録番号がオーバーしていない。
		//		return;
		//	componentArray.setOrPush(entity.chunkIndex_, component);
		//}

	private:
		//template<typename T, typename... Ts> void Process(Chunk *chunk) {
		//	/*size_t hash =std::hash<std::string>{}(typeid(T).name()); */
		//	//size_t size = sizeof(T);
		//	chunk->GetCustomArray<T>().push_back<T>();
		//	if constexpr (sizeof...(Ts) > 0) {
		//		Process<Ts...>(chunk);
		//	}
		//}
		//void CheckORCreateChunk() {}

		World *world_;
	};
}