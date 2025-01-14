/// @file EntityList.h
/// @brief エンティティのリスト
/// @author ONUKI seiya
#pragma once
#include "../World/ComponentArray/Chunk.h"
#include <ranges>


namespace ECS {
	template<bool IsConst = false>
	class EntityList {
	public:
		// Chunkの型
		using ChunkPtr = std::conditional_t<IsConst, const Chunk *, Chunk *>;

		EntityList() = default;
		EntityList(ChunkPtr chunk, uint32_t begin, uint32_t end) : chunk_(chunk), begin_(begin), end_(end) {}

		template<typename... Ts>
		auto View() const {
			return chunk_->View<Ts...>(begin_, end_);
		}

		ChunkPtr GetChunk() const { return chunk_; }

		auto ItrRange() const { return std::views::iota(begin_, end_); }

		uint32_t begin() const { return begin_; }
		uint32_t end() const { return end_; }

		//struct Iterator {
		//	EntityList *pEntityList_;

		//	uint32_t index_;
		//};

		//using iterator = Iterator;

		//std::ranges::subrange View() const { return { begin(), end() }; }

		//iterator begin() {
		//	return iterator{ this, begin_ };
		//}

		//iterator end() {
		//	return iterator{ this, end_ };
		//}

	private:
		// チャンク
		ChunkPtr chunk_;
		uint32_t begin_;
		uint32_t end_;

	};

}