#include "Enemy.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include <imgui.h>
#include "../../Utils/Math/Angle.h"
#include "../../Utils/Math/Math.hpp"

Enemy::~Enemy() {}

void Enemy::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);
	collider_.size = Vector3::one * 2.f;
}

void Enemy::Update(const float deltaTime) {
	static float rotateAngle = 0.2f * Angle::Dig2Rad;
	static float moveSpeed = 0.03f;
	ImGui::Begin("Enemy");
	ImGui::SliderAngle("RotateY", &rotateAngle);
	ImGui::SliderFloat("MoveSpeed", &moveSpeed, 0.f, 10.f);
	ImGui::End();

	transformOrigin_->translate +=
		TransformNormal(Vector3::front * moveSpeed, Matrix4x4::EulerRotate(transformOrigin_->rotate));
	transformOrigin_->rotate.y += rotateAngle;

	if (pPlatform_) {
		const OBB &obb = pPlatform_->GetCollider();
		transformOrigin_->translate.x = std::clamp(transformOrigin_->translate.x, -obb.size.x, obb.size.x);
		transformOrigin_->translate.z = std::clamp(transformOrigin_->translate.z, -obb.size.z, obb.size.z);
	}
	BaseCharacter::Update(deltaTime);
	collider_.SetMatrix(transformOrigin_->matWorld_);
}

void Enemy::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	modelMap_.at("body")->Draw(transformOrigin_, camera);
}

void Enemy::SetPlatform(const Platform *const platform) {
	if (platform == nullptr) {
		transformOrigin_->DisConnectParent();
		pPlatform_ = nullptr;
		return;
	}
	pPlatform_ = platform;

	transformOrigin_->SetParent(platform->GetWorldTransform());

}
