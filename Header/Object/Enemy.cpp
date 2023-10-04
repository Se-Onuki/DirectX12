#include "Enemy.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include <imgui.h>
#include "../../Utils/Math/Angle.h"
#include "../../Utils/Math/Math.hpp"

Enemy::~Enemy() {}

void Enemy::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);
}

void Enemy::Update() {
	static float rotateAngle = 2.f * Angle::Dig2Rad;
	static float moveSpeed = 0.3f;
	ImGui::Begin("Enemy");
	ImGui::SliderAngle("RotateY", &rotateAngle);
	ImGui::SliderFloat("MoveSpeed", &moveSpeed, 0.f, 10.f);
	ImGui::End();

	transformOrigin_.translate +=
		TransformNormal(Vector3::front * moveSpeed, transformOrigin_.matWorld_);
	transformOrigin_.rotate.y += rotateAngle;
	BaseCharacter::Update();
}

void Enemy::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	modelMap_.at("body")->Draw(transformOrigin_, camera);
}
