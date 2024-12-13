/// @file HealthComp.cpp
/// @brief 体力コンポーネント
/// @author ONUKI seiya
#include "HealthComp.h"
#include <algorithm>

void HealthComp::Reset() {
	nowHealth_ = maxHealth_;
}

void HealthComp::Update() {
	if (nowHealth_ <= 0.f) {
		object_->SetActive(false);
	}
}

void HealthComp::SetMaxHealth(const float value) {
	maxHealth_ = value;
}

void HealthComp::AddHealth(const float value) {
	// 0から最大値に収めつつ、体力を変更する。
	nowHealth_ = std::clamp(nowHealth_ + value, 0.f, maxHealth_);
}
