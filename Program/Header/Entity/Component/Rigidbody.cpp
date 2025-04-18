/// @file Rigidbody.cpp
/// @brief 物理挙動のコンポーネント
/// @author ONUKI seiya
#include "Rigidbody.h"
#include "Collider.h"

void Rigidbody::Init() {
	velocity_ = {};
	acceleration_ = {};
}

void Rigidbody::Update() {

	static auto *const collisionManager = CollisionManager::GetInstance();

	beforePos_ = transform_->translate;

	velocity_ += acceleration_;
	Vector3 fixVelocity = velocity_ * GetDeltaTime();

	const Vector3 maxSpeed = maxSpeed_ * GetDeltaTime();

	// 最大速度のポインタ(配列として扱う)
	const float *const maxSpeedPtr = maxSpeed.data();
	// 修正前の座標のポインタ
	float *const fixVelocityPtr = fixVelocity.data();
	// 各要素をclampする(最大速度が負数なら無効化)
	for (uint32_t i = 0u; i < 3u; ++i) {
		if (maxSpeedPtr[i] > 0.f) {
			fixVelocityPtr[i] = std::clamp(fixVelocityPtr[i], -maxSpeedPtr[i], maxSpeedPtr[i]);
		}
	}

	transform_->translate += fixVelocity;

	//const auto &boxArray = collisionManager->GetBox();

	//boxArray;

	isGround_ = false;

	if (isGround_) {
		velocity_.x = 0.f;

		if (velocity_.y < 0.f) {
			velocity_.y = 0.f;
		}
	}


	acceleration_ = {};
}
