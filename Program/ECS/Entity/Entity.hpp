#pragma once
#include <cstdint>

namespace ECS {

	class MultiArray;

	class Entity {
	public:
		MultiArray *arrayPtr_;
		uint32_t totalIndex_; // トータル番号
		uint32_t version_;

		//template<typename T> T& GetComp();
	};
}