#pragma once
#include <unordered_map>
#include <memory>
#include "../Archetype.h"
#include "ComponentArray/Chunk.h"
#include "../Entity/Prefab.h"

namespace ECS {



	template <typename T, bool IsConst = false>
	class ChunkTRange : public std::vector<ComponentData::TRange<T, IsConst>> {
	public:
		/*using Type = std::vector<ComponentData::TRange<T, IsConst>>;

		struct Iterator {
			ChunkTRange *range_;
			uint32_t chunkIndex_;
			uint32_t itr_;
			T &operator->() requires(std::is_same_v<IsConst, false>) {
				return range_->data_[chunkIndex_]->GetComp<T>(itr_);
			}
		};

		friend Iterator;

	public:
		ChunkTRange() = default;
		ChunkTRange(Type &&item) : data_(std::move(item)) {}




	private:
		Type data_;
		uint32_t chunkIndex_;
		uint32_t itr_;*/

	public:

		T &At(uint32_t index) /*requires(std::is_same_v<IsConst, false>)*/ {

			for (ComponentData::TRange<T, IsConst> &chunk : *this) {
				if (index > chunk.size()) {
					index -= chunk.size();
					continue;
				}
				return chunk[index];
			}
			assert(0 and "範囲外アクセス");
			return this->front()[0];
		}

		/*const T &At(uint32_t index) const requires(std::is_same_v<IsConst, true>) {

			for (const ComponentData::TRange<T, IsConst> &chunk : *this) {
				if (index > chunk.size()) {
					index -= chunk.size();
					continue;
				}
				else {
					return chunk[index];
				}
			}
		}*/
	};
	template <bool IsConst = false>
	class ChunkSet : public std::conditional_t<IsConst, std::vector<const Chunk *>, std::vector<Chunk *>> {
	public:

		using Type = std::conditional_t<IsConst, std::vector<const Chunk *>, std::vector<Chunk *>>;

	public:

		template <typename T>
		ChunkTRange<T, IsConst> GetRange() const {
			ChunkTRange<T, IsConst> result;
			result.resize(this->size());

			std::transform(this->cbegin(), this->cend(), result.begin(), [](auto *chunk) {return chunk->GetComponent<T>(); });

			return result;
		}

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

		template<typename T>
		uint32_t CountIf(const T &data) const {

			// chuckの配列が存在すれば､それに格納されてる数を返す
			return this->empty() ? 0u : std::accumulate(this->cbegin(), this->cend(), 0u, [&data](uint32_t acc, const Chunk *chunk)->uint32_t
				{
					// 空なら終わる
					if (not chunk) { return acc; }

					// chunkのView
					auto view = chunk->GetComponent<T>();

					// chunkにアクセスできるならその数を足す
					return std::count_if(view.begin(), view.end(), [&data](const T &i) {return i == data; }) + acc;
				}
			);
		}
		template<typename T>
		std::pair<std::vector<bool>, size_t> CountIfFlag(const T &data) const {
			// ヒット数
			size_t count = 0;
			// 要素数の数だけのメモリを確保する
			std::vector<bool> flag(Count());
			auto itr = flag.begin();
			// チャンクを走査
			for (const Chunk *chank : *this) {
				// コンポーネントを取得して返す
				for (const T &value : chank->GetComponent<T>()) {
					*itr = value == data;
					++itr;
					if (value == data) { count++; }
				}
			}
			return { std::move(flag), count };
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