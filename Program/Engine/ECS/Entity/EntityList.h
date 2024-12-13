/// @file EntityList.h
/// @brief エンティティのリスト
/// @author ONUKI seiya
#pragma once
#include "../World/ComponentArray/Chunk.h"

namespace ECS {
	template<bool IsConst = false>
	class EntityList {
	public:
		// Chunkの型
		using Type = std::conditional_t<IsConst, const Chunk *, Chunk *>;

		EntityList() = default;
		EntityList(Type chunk, uint32_t begin, uint32_t end) : chunk_(chunk), begin_(begin), end_(end) {}



	private:
		// チャンク
		Type chunk_;
		uint32_t begin_;
		uint32_t end_;

	};

}