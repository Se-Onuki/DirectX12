#pragma once
#include <unordered_map>
#include <memory>
#include "../Archetype.h"
#include "ComponentArray/Chunk.h"

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
			if (not chunk) { return chunk; }

			auto &&itr = chunkMap_.insert({ archetype, std::make_unique<Chunk>(archetype) }).first;
			return itr->second.get();

		}




	private:

		std::unordered_map<Archetype, std::unique_ptr<Chunk>> chunkMap_;

	};

}