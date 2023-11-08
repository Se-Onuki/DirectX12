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
	static float rotateAngle = 2.f * Angle::Dig2Rad;
	static float moveSpeed = 0.3f;
	ImGui::Begin("Enemy");
	ImGui::SliderAngle("RotateY", &rotateAngle);
	ImGui::SliderFloat("MoveSpeed", &moveSpeed, 0.f, 10.f);
	ImGui::End();

	transformOrigin_->translate +=
		TransformNormal(Vector3::front * moveSpeed, Matrix4x4::EulerRotate(transformOrigin_->rotate));
	transformOrigin_->rotate.y += rotateAngle;
	transformOrigin_->translate.x = std::clamp(transformOrigin_->translate.x, -10.f, 10.f);
	transformOrigin_->translate.z = std::clamp(transformOrigin_->translate.z, -10.f, 10.f);

	BaseCharacter::Update(deltaTime);
	collider_.SetMatrix(transformOrigin_->matWorld_);
}

void Enemy::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	modelMap_.at("body")->Draw(transformOrigin_, camera);
}
