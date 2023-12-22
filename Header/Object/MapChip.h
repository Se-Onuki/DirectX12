#pragma once
#include <list>
#include "../Collision/Collision.h"
#include "Block/BlockManager.h"
#include "../../Utils/IO/CSV.h"

class MapChip
{
public:
	MapChip() = default;
	~MapChip() = default;

	void Init();

	void Load(const SoLib::IO::CSV &csv);

	void Update(const float deltaTime);

	void Draw();

private:

	BlockManager *pBlockManager_;
	Model *pModel_;

	//std::

	std::list<AABB> boxList_;

};