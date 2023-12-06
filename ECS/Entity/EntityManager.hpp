#pragma once

#include <typeinfo>

#include "Header/Chunk/Chunk.hpp"
#include "Header/Component/Component.hpp"
#include "Header/Entity/Entity.hpp"
#include "Header/World/World.hpp"

class EntityManager {
public:
	EntityManager(World* world);

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

	template<typename... Ts> const Entity CreateEntity() {
		Entity entity{};
		Archetype archetype;
		archetype.AddClassData<Ts...>();

		entity.chunkId_ = world_->FindMatchChunk(archetype);

		if (entity.chunkId_ == UINT32_MAX) {
			entity.chunkId_ = world_->CreateChunk(archetype);
		}
		Chunk* chunk = world_->GetChunk(entity.chunkId_);
		entity.chunkIndex_ = chunk->entityCount_++;

		Process<Ts...>(chunk);

		return entity;
	}

	Entity CreateEntity(const uint32_t& chunkId) {
		Chunk* targetChunk = world_->GetChunk(chunkId);
		targetChunk->entityCount_++;
		if (targetChunk->maxCount_ < targetChunk->entityCount_) {
		}
		Entity{chunkId, targetChunk->entityCount_};
	}

	// std::vector<Chunk&> FindHitChunk(const Archetype& archetype);

	template<typename T> T& GetComponent(Entity entity) {
		return world_->GetChunk(entity.chunkId_)->GetArray<T>()[entity.chunkIndex_];
	}

	template<typename T> void SetComponent(Entity entity, T& component) {
		Chunk* chunk = world_->GetChunk(entity.chunkId_);
		CustomArray& componentArray = chunk->GetCustomArray<T>();
		if (componentArray.size() == chunk->size() &&
		    componentArray.size() <=
		        entity.chunkIndex_) // 配列の要素数がchunkの制御番号を超えていない +
		                            // エンティティの登録番号がオーバーしていない。
			return;
		componentArray.setOrPush(entity.chunkIndex_, component);
	}

private:
	template<typename T, typename... Ts> void Process(Chunk* chunk) {
		size_t hash = std::hash<std::string>{}(typeid(T).name());
		size_t size = sizeof(T);
		chunk->GetCustomArray<T>().push_back<T>();
		if constexpr (sizeof...(Ts) > 0) {
			Process<Ts...>(chunk);
		}
	}
	void CheckORCreateChunk() {}
	World* world_;
};