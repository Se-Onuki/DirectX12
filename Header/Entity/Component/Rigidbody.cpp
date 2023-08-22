#include "Rigidbody.h"

const float Rigidbody::deltaTime_ = 1.f / 60.f;

void Rigidbody::Init() {
	velocity_ = {};
	acceleration_ = {};
}

void Rigidbody::Update() {

	velocity_ += acceleration_ * deltaTime_;
	object_->transform_.translate += velocity_ * deltaTime_;

	// y座標
	float &valueY = object_->transform_.translate.y;
	isGround_ = valueY <= 0.f; // 地面以下だった場合地面にいる
	if (isGround_) {
		valueY = 0.f;
		if (velocity_.y < 0.f) {
			velocity_.y = 0.f;
		}
	}


	acceleration_ = {};
}
