#include "LevelElementManager.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../Utils/SoLib/SoLib_Easing.h"
#include "../../Utils/SoLib/SoLib_ImGui.h"

void LevelElementManager::Init() {
	if (lineStart_->translate == lineEnd_->translate) {
		lineEnd_->translate.z = lineStart_->translate.z + 1.f;
	}
}

void LevelElementManager::Update([[maybe_unused]] float deltaTime) {
	for (auto &[key, platform] : blockCollider_) {
		platform.Update(deltaTime);
	}

	ImGui::Begin("StageLine");
	SoLib::ImGuiWidget("StartPos", &lineStart_->translate);
	SoLib::ImGuiWidget("EndPos", &lineEnd_->translate);
	ImGui::End();

	lineStart_->UpdateMatrix();
	lineEnd_->UpdateMatrix();
	stageLine_.origin = lineStart_->GetGrobalPos();
	stageLine_.diff = lineEnd_->GetGrobalPos() - stageLine_.origin;

}

void LevelElementManager::Draw([[maybe_unused]] const Camera3D &camera) const {
	static const ModelManager *const modelManager = ModelManager::GetInstance();
	const Model *const model = modelManager->GetModel("Box");

	for (const auto &[key, platform] : blockCollider_) {
		platform.Draw(model, camera);
	}
}
//
//void LevelElementManager::Draw(const Camera3D &camera) {
//	/*static const ModelManager *const modelManager = ModelManager::GetInstance();
//	const Model *const model = modelManager->GetModel("Box");
//
//	for (const auto &[key, platform] : blockCollider_) {
//		platform.Draw(model, camera);
//	}*/
//}

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

LevelElementManager::Platform *const LevelElementManager::GetPlatform(int32_t index) {
	if (index < 0) {
		return nullptr;
	}

	PlatformMap::iterator itPlatform = blockCollider_.find(index);
	if (itPlatform == blockCollider_.end()) {
		return nullptr;
	}
	return &itPlatform->second;
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


	if (timer_.IsActive()) {
		center_.rotate = Angle::Lerp(startRot_, targetRot_, SoLib::easeInOutQuad(timer_.GetProgress()));
		center_.UpdateMatrix();
		for (auto &box : boxList_) {
			box.transform_->UpdateMatrix();

		}

	}
	if (timer_.IsActive() && timer_.IsFinish()) {
		startRot_ = targetRot_;
		this->CalcCollision();
	}
}

void LevelElementManager::Platform::AddRotate(const float targetRot) {
	targetRot_ = Angle::Mod(rotateAxis_ * targetRot + startRot_);
	timer_.Start(vLerpTime_);
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
