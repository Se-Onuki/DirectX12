#include "Enemy.h"
#include "../../Engine/DirectBase/Model/Model.h"
#include <imgui.h>
#include "../../Utils/Math/Angle.h"
#include "../../Utils/Math/Math.hpp"
#include "Particle/TestParticle.h"

uint32_t Enemy::sNextNumber_ = 0u;

Enemy::Enemy() : entityNumber_(sNextNumber_++) {
}

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
void Enemy::StartDeadAnim(const Vector3 &playerPos) {
	auto *particlePtr = ParticleManager::GetInstance()->AddParticle(ModelManager::GetInstance()->GetModel("enemyBody"), std::make_unique<TestParticle>(this->GetWorldTransform()->GetGrobalPos()));
	particlePtr->velocity_ = (this->GetWorldTransform()->GetGrobalPos() - playerPos).Nomalize() * 30.f;
	particlePtr->SetAliveTime(1.f);
	particlePtr->Init();

	this->SetIsAlive(false);
}

void Enemy::HitWeapon(const Vector3 &playerPos) {
	AddHealth(-1);

	for (uint32_t i = 0u; i < 10u; i++) {
		TestParticle *particlePtr = dynamic_cast<TestParticle *>(ParticleManager::GetInstance()->AddParticle(ModelManager::GetInstance()->GetModel("box"), std::make_unique<TestParticle>(this->GetWorldTransform()->GetGrobalPos())));
		// 乱数で角度を設定
		Vector3 euler = Vector3{ Random::GetRandom<float>(-Angle::PI * 0.5f,Angle::PI * 0.5f),  Random::GetRandom<float>(-Angle::PI,Angle::PI), 0.f };
		particlePtr->velocity_ = Vector3::front * Random::GetRandom<float>(15.f, 30.f) * Matrix4x4::EulerRotate(euler);
		particlePtr->SetAliveTime(0.5f);
		particlePtr->SetStartColor(Vector4{ 1.f,0.5f,0.5f,0.5f });
		particlePtr->SetEndColor(Vector4{ 1.f,0.5f,0.5f,0.f });
		particlePtr->Init();
	}
	if (health_ <= 0) {
		StartDeadAnim(playerPos);
	}
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
