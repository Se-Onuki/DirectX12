#include "MapChip.h"

void MapChip::Init() {
	pBlockManager_ = BlockManager::GetInstance();
	pModel_ = ModelManager::GetInstance()->GetModel("Block");
}

void MapChip::Update([[maybe_unused]] const float deltaTime) {
}

void MapChip::Draw() {

	for (auto &box : boxList_) {
		IBlock block = IBlock{ .transformMat_ = box.TransMat(),.color_ = Vector4::one };
		pBlockManager_->AddBox(pModel_, std::move(block));
	}
}
