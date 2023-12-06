#pragma once
#include <cstdint>

class Entity {
public:
	uint32_t chunkId_; // 所属チャンク
	uint32_t chunkIndex_; // チャンク内番号

	//template<typename T> T& GetComp();
};
