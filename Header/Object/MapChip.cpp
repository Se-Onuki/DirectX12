#include "MapChip.h"

void MapChip::Init() {
	pBlockManager_ = BlockManager::GetInstance();
	pModel_ = ModelManager::GetInstance()->GetModel("Block");
}

void MapChip::Load(const SoLib::IO::CSV &csv) {

	mapData_.Resize(csv.GetHeight(), csv.GetWidth());
	auto mapItr = mapData_.begin();
	for (auto &line : csv) {
		for (auto &item : line) {
			if (item == "") { *mapItr = 0u; }
			else { *mapItr = static_cast<uint32_t>(std::stoul(item)); }
			++mapItr;
		}
	}
}

void MapChip::Update([[maybe_unused]] const float deltaTime) {
}

void MapChip::Draw() {

	for (auto &box : boxList_) {
		IBlock block = IBlock{ .transformMat_ = box.TransMat(),.color_ = Vector4::one };
		pBlockManager_->AddBox(pModel_, std::move(block));
	}
}
