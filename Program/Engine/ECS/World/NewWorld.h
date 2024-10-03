#pragma once
#include <unordered_map>
#include <memory>
#include "../Archetype.h"
#include "ComponentArray/Chunk.h"
#include "../Entity/Prefab.h"

namespace ECS {

	template <bool IsConst = false>
	class ChunkSet : public std::conditional_t<IsConst, std::vector<const Chunk *>, std::vector<Chunk *>> {
	public:

		using Type = std::conditional_t<IsConst, std::vector<const Chunk *>, std::vector<Chunk *>>;

	public:

		/// @brief エンティティの数を計算する
		/// @return エンティティの数
		uint32_t Count() const {
			// chuckの配列が存在すれば､それに格納されてる数を返す
			return this->empty() ? 0u : std::accumulate(this->cbegin(), this->cend(), 0u, [](uint32_t acc, const Chunk *i)->uint32_t
				{
					// chunkにアクセスできるならその数を足す
					return i ? i->size() + acc : acc;
				}
			);
		}

	private:
	};

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

		ChunkSet<false> GetAccessableChunk(const Archetype &archetype) {
			ChunkSet<false> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key) { result.push_back(chunk.get()); }
			}
			return result;
		}

		ChunkSet<true> GetAccessableChunk(const Archetype &archetype) const {
			ChunkSet<true> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key) { result.push_back(chunk.get()); }
			}
			return result;
		}

		ChunkSet<false> GetAccessableChunk(const Archetype &archetype, const ECS::ComponentRegistry::ComponentFlag exclusions) {
			ChunkSet<false> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key and (key.compFlag_.Get() & exclusions.Get()).none()) { result.push_back(chunk.get()); }
			}
			return result;
		}

		ChunkSet<true> GetAccessableChunk(const Archetype &archetype, const ECS::ComponentRegistry::ComponentFlag exclusions) const {
			ChunkSet<true> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key and (key.compFlag_.Get() & exclusions.Get()).none()) { result.push_back(chunk.get()); }
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