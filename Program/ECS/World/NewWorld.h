#pragma once
#include <unordered_map>
#include <memory>
#include "../Archetype.h"
#include "ComponentArray/Chunk.h"
#include "../Entity/Prefab.h"

namespace ECS {

	class World {
	public:

		World() = default;

		Chunk *GetChunk(const Archetype &archetype) {
			auto itr = chunkMap_.find(archetype);
			return itr == chunkMap_.end() ? nullptr : itr->second.get();
		}

		Chunk *CreateOrGetChunk(const Archetype &archetype) {
			auto chunk = GetChunk(archetype);
			if (chunk) { return chunk; }

			auto &&itr = chunkMap_.insert({ archetype, std::make_unique<Chunk>(archetype) }).first;
			return itr->second.get();

		}

		const EntityClass &CreateEntity(const Archetype &archetype);

		const EntityClass &CreateEntity(const Prefab &prefab);
		std::vector<EntityClass> CreateEntity(const Archetype &archetype, uint32_t count);
		std::vector<EntityClass> CreateEntity(const Prefab &prefab, uint32_t count);

		std::vector<Chunk *> GetAccessableChunk(const Archetype &archetype) {
			std::vector<Chunk *> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key) { result.push_back(chunk.get()); }
			}
			return result;
		}

		template<typename T, typename Predicate>
		void erase_if(Predicate pred);

		uint32_t size() {
			uint32_t result = 0;
			for (const auto &[key, chunk] : chunkMap_) {
				result += chunk->size();
			}
			return result;
		}

	private:

		std::unordered_map<Archetype, std::unique_ptr<Chunk>> chunkMap_;

	};

	template<typename T, typename Predicate>
	inline void World::erase_if(Predicate pred)
	{
		Archetype archetype;
		archetype.AddClassData<T>();
		auto chunkList = GetAccessableChunk(archetype);
		std::for_each(chunkList.begin(), chunkList.end(), [pred](Chunk *chunk)
			{
				std::erase_if(chunk->View<T>(), pred);
			}
		);

	}

}