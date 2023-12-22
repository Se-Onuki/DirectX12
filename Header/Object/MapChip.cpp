#include "MapChip.h"

void MapChip::Init() {
	pBlockManager_ = BlockManager::GetInstance();
	pModel_ = ModelManager::GetInstance()->GetModel("Block");
}

void MapChip::Load(const SoLib::IO::CSV &csv) {

	mapData_.Resize(csv.GetHeight(), csv.GetWidth());
	auto mapItr = mapData_.begin();
	for (auto lineItr = csv.Get().rbegin(); lineItr != csv.Get().rend(); ++lineItr) {
		for (auto &item : *lineItr) {
			if (item == "") { *mapItr = 0u; }
			else { *mapItr = static_cast<uint32_t>(std::stoul(item)); }
			++mapItr;
		}
	}
}

void MapChip::Update([[maybe_unused]] const float deltaTime) {
}

void MapChip::Draw() {
	const Vector3 beginPos = Vector3{ -static_cast<float>(mapData_.GetCols()) + 1.f,-static_cast<float>(mapData_.GetRows()) + 1.f, 0.f } *(0.5f * kChipScale_);
	Vector3 offset{};

	const Vector3 boxRadius = Vector3::one * kChipScale_ * kBoxScale;

	boxList_.clear();

	for (auto &line : mapData_.get()) {
		offset.x = 0.f;
		for (auto item : line) {
			if (item) {
				boxList_.push_back(AABB::Create(beginPos + offset, boxRadius));
			}
			offset.x += kChipScale_;
		}
		offset.y += kChipScale_;
	}

	for (auto &box : boxList_) {
		IBlock block = IBlock{ .transformMat_ = box.TransMat(),.color_ = 0xFFFFFFFF };
		pBlockManager_->AddBox(pModel_, std::move(block));
	}
}
