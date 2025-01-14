/// @file NewWorld.h
/// @brief ECSのワールド
/// @author ONUKI seiya
#pragma once
#include <unordered_map>
#include <memory>
#include "../Archetype.h"
#include "ComponentArray/Chunk.h"
#include "../Entity/Prefab.h"
#include "../Entity/EntityList.h"

namespace ECS {

	template <typename T, bool IsConst = false>
	class ChunkTRange : public std::vector<ComponentSpan::TRange<T, IsConst>> {
	public:

	public:


		/// @brief アクセス
		/// @param index アクセスするインデックス
		/// @return アクセスした値
		T &At(uint32_t index) requires(IsConst == false) {

			for (ComponentSpan::TRange<T, IsConst> &chunk : *this) {
				if (index >= chunk.size()) {
					index -= chunk.size();
					continue;
				}
				return chunk[index];
			}
			assert(0 and "範囲外アクセス");
			return this->front()[0];
		}

		/// @brief アクセス
		/// @param index アクセスするインデックス
		/// @return アクセスした値
		const T &At(uint32_t index) const {

			for (const ComponentSpan::TRange<T, IsConst> &chunk : *this) {
				if (index >= chunk.size()) {
					index -= chunk.size();
					continue;
				}
				else {
					return chunk[index];
				}
			}
			assert(0 and "範囲外アクセス");
			return this->front()[0];
		}
	};

	//template<bool IsConst, typename... Ts>
	//struct 



	template <bool IsConst = false>
	class ChunkSet : public std::conditional_t<IsConst, std::vector<const Chunk *>, std::vector<Chunk *>> {
	public:

		using Type = std::conditional_t<IsConst, std::vector<const Chunk *>, std::vector<Chunk *>>;

	public:

		/// @brief チャンクの範囲を返す
		/// @return チャンクの範囲
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

		/// @brief 条件に一致した値の数を返す
		/// @tparam T 比較する型
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

		/// @brief 一致した値の並びと数を返す
		/// @tparam T 比較する型
		/// @param data 比較する値
		/// @return 一致しているかをtrueで返す
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
					if (*itr++ = value == data) { count++; }
				}
			}
			return { std::move(flag), count };
		}

		/// @brief 一致した値の並びと数を返す
		/// @tparam T 比較する型
		/// @param data 比較する値
		/// @return 一致しているかをtrueで返す
		template<typename T, typename Pred>
		std::pair<std::vector<bool>, size_t> CountIfFlag(const Pred &pred) const {
			// ヒット数
			size_t count = 0;
			// 要素数の数だけのメモリを確保する
			std::vector<bool> flag(Count());
			auto itr = flag.begin();
			// チャンクを走査
			for (const Chunk *chank : *this) {
				// コンポーネントを取得して返す
				for (const T &value : chank->GetComponent<T>()) {
					if (*itr++ = pred(value)) { count++; }
				}
			}
			return { std::move(flag), count };
		}

	private:
	};

	class World {
	public:

		World() = default;

		/// @brief チャンクを返す
		Chunk *GetChunk(const Archetype &archetype) {
			auto itr = chunkMap_.find(archetype);
			return itr == chunkMap_.end() ? nullptr : itr->second.get();
		}

		/// @brief チャンクを返すか､生成する
		Chunk *CreateOrGetChunk(const Archetype &archetype) {
			auto chunk = GetChunk(archetype);
			if (chunk) { return chunk; }

			auto &&itr = chunkMap_.insert({ archetype, std::make_unique<Chunk>(archetype) }).first;
			return itr->second.get();

		}

		/// @brief エンティティを生成する
		const EntityClass &CreateEntity(const Archetype &archetype);

		const EntityClass &CreateEntity(const Prefab &prefab);
		EntityList<false> CreateEntity(const Archetype &archetype, uint32_t count);
		EntityList<false> CreateEntity(const Prefab &prefab, uint32_t count);

		/// @brief アクセスできるチャンクを返す
		ChunkSet<false> GetAccessableChunk(const Archetype &archetype) {
			ChunkSet<false> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key and chunk and chunk->size()) { result.push_back(chunk.get()); }
			}
			return result;
		}
		ChunkSet<true> GetAccessableChunk(const Archetype &archetype) const {
			ChunkSet<true> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key and chunk and chunk->size()) { result.push_back(chunk.get()); }
			}
			return result;
		}

		ChunkSet<false> GetAccessableChunk(const Archetype &archetype, const ECS::ComponentRegistry::ComponentFlag exclusions) {
			ChunkSet<false> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key and (key.required_.Get() & exclusions.Get()).none() and chunk and chunk->size()) { result.push_back(chunk.get()); }
			}
			return result;
		}

		ChunkSet<true> GetAccessableChunk(const Archetype &archetype, const ECS::ComponentRegistry::ComponentFlag exclusions) const {
			ChunkSet<true> result;
			for (const auto &[key, chunk] : chunkMap_) {
				if (archetype <= key and (key.required_.Get() & exclusions.Get()).none() and chunk and chunk->size()) { result.push_back(chunk.get()); }
			}
			return result;
		}

		/// @brief 指定した条件を持つエンティティを削除する
		/// @param pred 条件の関数
		template<typename T, typename Predicate>
		void erase_if(const Predicate &pred);

		/// @brief エンティティの数を返す
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
	inline void World::erase_if(const Predicate &pred)
	{
		Archetype archetype;
		archetype.AddClassData<T>();
		auto chunkList = GetAccessableChunk(archetype);
		std::for_each(chunkList.begin(), chunkList.end(), [pred](Chunk *chunk)
			{
				chunk->erase_if<T>(pred);
			}
		);

	}

}