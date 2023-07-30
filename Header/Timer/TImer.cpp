#include "Timer.h"
#include <algorithm>

bool Timer::Update() {
	if (isFinish_)
		return false;
	AddFlame();
	if (nowFlame_ >= goalFlame_) {
		isFinish_ = true;
	}
	return true;
}

void Timer::AddFlame() {
	nowFlame_++;
}

void Timer::Reset() {
	nowFlame_ = 0u;
	isFinish_ = false;
}

void Timer::Init(uint32_t goal) {
	Reset();
	SetGoal(goal);
}

float Timer::GetProgress() const {
	return std::clamp((float)nowFlame_ / goalFlame_, 0.f, 1.f);
}

bool FunctionTimer::Update() {
	bool isActive = Timer::Update(); // 実行中であるか
	if (isActive && IsFinish())	// 実行中である & ゴールに到達している => 実行
		function_();
	return isActive;
}