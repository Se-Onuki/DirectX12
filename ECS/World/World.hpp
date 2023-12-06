#pragma once
#include "Header/Chunk/Chunk.hpp"
#include <functional>
#include <list>

class SystemBase;
class EntityManager;

class World {
public:
	static World* GetInstance() {
		static World instance;
		return &instance;
	}
	~World();

	EntityManager* GetEntityManager() { return entityManager_; }

	uint32_t CreateChunk(const Archetype& archetype) {
		chunkList_.emplace_back(archetype, 16);
		return chunkList_.size() - 1;
	}
	uint32_t FindMatchChunk(const Archetype& archetype) {
		for (uint32_t i = 0; i < chunkList_.size(); ++i) {
			if (chunkList_[i].GetArchetype() == archetype) {
				return i;
			}
		}
		return UINT32_MAX;
	}

	Chunk* GetChunk(const uint32_t& index) { return &chunkList_[index]; }

	std::vector<Chunk> chunkList_ = {};

	template<typename A> void ForEach(std::function<void(A&)> func);
	template<typename A, typename B> void ForEach(std::function<void(A&, B&)> func);
	template<typename A, typename B, typename C> void ForEach(std::function<void(A&, B&, C&)> func);

private:
	World();
	World(const World&) = delete;
	World& operator==(const World&) = delete;
	EntityManager* entityManager_;
};

template<typename A> inline void World::ForEach(std::function<void(A&)> func) {
	Archetype arType;
	arType.AddClassData<A>();
	for (Chunk& chunk : chunkList_) {
		if (!(arType <= chunk.GetArchetype()))
			continue;
		A* dataA = chunk.GetArray<A>();
		for (uint32_t i = 0; i < chunk.entityCount_; i++) {
			func(dataA[i]);
		}
	}
}

template<typename A, typename B> inline void World::ForEach(std::function<void(A&, B&)> func) {
	Archetype arType;
	arType.AddClassData<A, B>();
	for (Chunk& chunk : chunkList_) {
		if (!(arType <= chunk.GetArchetype()))
			continue;
		A* dataA = chunk.GetArray<A>();
		B* dataB = chunk.GetArray<B>();
		for (uint32_t i = 0; i < chunk.entityCount_; i++) {
			func(dataA[i], dataB[i]);
		}
	}
}

template<typename A, typename B, typename C>
inline void World::ForEach(std::function<void(A&, B&, C&)> func) {
	Archetype arType;
	arType.AddClassData<A, B, C>();
	for (Chunk& chunk : chunkList_) {
		if (!(arType <= chunk.GetArchetype()))
			continue;
		A* dataA = chunk.GetArray<A>();
		B* dataB = chunk.GetArray<B>();
		C* dataC = chunk.GetArray<C>();
		for (uint32_t i = 0; i < chunk.entityCount_; i++) {
			func(dataA[i], dataB[i], dataC[i]);
		}
	}
}
