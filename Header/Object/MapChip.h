#pragma once
#include <list>
#include "../Collision/Collision.h"
#include "Block/BlockManager.h"

class MapChip
{
public:
	MapChip() = default;
	~MapChip() = default;

	void Init();

	//void Load(const )

	void Update(const float deltaTime);

	void Draw();

private:

	BlockManager *pBlockManager_;
	Model *pModel_;

	std::list<AABB> boxList_;

};