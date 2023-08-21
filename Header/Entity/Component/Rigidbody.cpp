#include "Rigidbody.h"

void Rigidbody::Init() {
	velocity_ = {};
	acceleration_ = {};
}

void Rigidbody::Update() {
	velocity_ += acceleration_;
	object_->transform_.translate += velocity_;

	acceleration_ = {};
}
