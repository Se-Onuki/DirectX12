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

void LevelElementManager::AddBlock(const uint32_t key, const AABB &box) {
	blockCollider_[key].AddBox(box);
}

//void LevelElementManager::AddBlock(const std::string &key, Entity *const entity) {
//	blockEntity_[key].emplace_back(entity);
//}

void LevelElementManager::Platform::AddBox(const AABB &box) {
	referenceBox_.push_back(box);
	transform_.emplace_back();

	auto &newTransform = transform_.back();
	newTransform->translate = box.GetCentor();
	newTransform->scale = box.GetRadius();
}

void LevelElementManager::Platform::CalcCollision() {
	const auto &rotateMat = Matrix4x4::EulerRotate(rotate_);
	collisionBox_.clear();
	auto itTransform = transform_.begin();
	for (auto &box : referenceBox_) {
		AABB newBox = box;
		newBox.min *= rotateMat;
		newBox.max *= rotateMat;
		collisionBox_.push_back(newBox.Swaping());


		(*itTransform)->translate = newBox.GetCentor();
		(*itTransform)->scale = newBox.GetRadius();
		(*itTransform)->UpdateMatrix();

		++itTransform;
	}
}

void LevelElementManager::Platform::Draw(const Model *const model, const Camera3D &camera) const {
	for (const auto &transform : transform_) {
		model->Draw(transform, camera);
	}
}
