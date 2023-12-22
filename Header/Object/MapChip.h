#pragma once
#include <list>
#include "../Collision/Collision.h"
#include "Block/BlockManager.h"
#include "../../Utils/IO/CSV.h"
#include "../../Utils/Containers/Array2D.h"

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

	const float kChipScale_ = 1.f;
	const float kBoxScale = 0.5f;

	BlockManager *pBlockManager_;
	Model *pModel_;

	SoLib::Containers::Array2D<uint32_t> mapData_;

	std::list<AABB> boxList_;

};