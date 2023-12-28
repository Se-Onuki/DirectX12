#pragma once
#include <cstdint>

namespace ECS {

	class MultiArray;

	class Entity {
	public:
		MultiArray *arrayPtr_;
		size_t chunkIndex_; // チャンク内番号
		uint32_t version_;



		//template<typename T> T& GetComp();
	};
}