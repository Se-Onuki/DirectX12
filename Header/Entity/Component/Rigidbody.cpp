#include "Rigidbody.h"

void Rigidbody::Init() {
	velocity_ = {};
	acceleration_ = {};
}

void Rigidbody::Update() {

	beforePos_ = object_->transform_.translate;
	// const Vector3 &afterPos = object_->transform_.translate;

	velocity_ += acceleration_;
	Vector3 fixVelocity = velocity_ * object_->GetDeltaTime();

	const Vector3 maxSpeed = maxSpeed_ * object_->GetDeltaTime();

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

	object_->transform_.translate += fixVelocity;

	isGround_ = false;

	if (isGround_) {
		velocity_.x = 0.f;

		if (velocity_.y < 0.f) {
			velocity_.y = 0.f;
		}
	}


	acceleration_ = {};
}
