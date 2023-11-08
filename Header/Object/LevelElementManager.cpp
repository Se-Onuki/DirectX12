#include "LevelElementManager.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"

void LevelElementManager::Init() {
}

void LevelElementManager::Update([[maybe_unused]] float deltaTime) {


}

void LevelElementManager::Draw([[maybe_unused]] const Camera3D &camera) const {
}

void LevelElementManager::Draw(const Camera3D &camera) {
	static const ModelManager *const modelManager = ModelManager::GetInstance();
	const Model *const model = modelManager->GetModel("Box");

	for (const auto &[key, platform] : blockCollider_) {
		platform.Draw(model, camera);
	}
}

void LevelElementManager::CalcCollision(const uint32_t key) {
	blockCollider_[key].CalcCollision();
}

void LevelElementManager::CalcCollision() {
	for (auto &[key, platform] : blockCollider_) {
		platform.CalcCollision();
	}
}

void LevelElementManager::AddBlock(const uint32_t key, const AABB &box) {
	blockCollider_[key].AddBox(box);
}

//void LevelElementManager::AddBlock(const std::string &key, Entity *const entity) {
//	blockEntity_[key].emplace_back(entity);
//}

void LevelElementManager::Platform::AddBox(const AABB &aabb) {
	boxList_.emplace_back(aabb);
	auto &box = boxList_.back();
	box.transform_->parent_ = &center_;
}

void LevelElementManager::Platform::CalcCollision() {
	center_.CalcMatrix();
	const auto &affineMat = center_.matWorld_;
	collisionBox_.clear();

	for (auto &box : boxList_) {
		AABB newBox = box.referenceBox_;
		newBox.min *= affineMat;
		newBox.max *= affineMat;
		collisionBox_.push_back(newBox.Swaping());


		box.transform_->UpdateMatrix();

	}
}

void LevelElementManager::Platform::Update(float deltaTime) {
	timer_.Update(deltaTime);

	if (timer_.IsActive() && timer_.IsFinish()) {
		center_.rotate = Angle::Lerp(startRot_, targetRot_, timer_.GetProgress());
	}

	if (timer_.IsActive()) {
		this->CalcCollision();
	}
}

void LevelElementManager::Platform::Draw(const Model *const model, const Camera3D &camera) const {
	for (const auto &box : boxList_) {
		model->Draw(box.transform_, camera);
	}
}

LevelElementManager::Box::Box(const AABB &aabb) {
	transform_->translate = aabb.GetCentor();
	transform_->scale = aabb.GetRadius();

	referenceBox_ = aabb;
}
