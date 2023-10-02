#include "SoLib_Timer.h"
#include <algorithm>

namespace SoLib {
	bool Timer::Update() {
		if (isFinish_) {
			isActive_ = false;
			return false;
		}
		AddFlame();
		if (nowFlame_ >= goalFlame_) {
			isFinish_ = true;
		}
		return true;
	}

	void Timer::AddFlame() {
		nowFlame_++;
	}

	void Timer::Start() {
		nowFlame_ = 0u;
		isFinish_ = false;
		isActive_ = true;
	}

	void Timer::Start(uint32_t goal) {
		Start();
		SetGoal(goal);
	}

	void Timer::Clear() {
		nowFlame_ = 0u;
		isFinish_ = true;
		isActive_ = false;
		goalFlame_ = 0u;
	}

	float Timer::GetProgress() const {
		return std::clamp((float)nowFlame_ / goalFlame_, 0.f, 1.f);
	}



	bool RealTimer::Update() {
		if (isFinish_) {
			isActive_ = false;
			return false;
		}

		auto now = std::chrono::steady_clock::now();
		duration_ = std::chrono::duration_cast<std::chrono::duration<float>>(now - startTime_);

		if (duration_ >= goalDuration_) {
			isFinish_ = true;
		}

		return true;
	}

	void RealTimer::Start() {
		startTime_ = std::chrono::steady_clock::now();
		duration_ = std::chrono::duration<float>(0.f);
		isFinish_ = false;
		isActive_ = true;
	}

	void RealTimer::Start(float seconds) {
		Start();
		goalDuration_ = std::chrono::duration<float>(seconds);
	}

	void RealTimer::Clear() {
		duration_ = std::chrono::duration<float>(0.f);
		isFinish_ = true;
		isActive_ = false;
		goalDuration_ = std::chrono::duration<float>(0.f);
	}

	// 割合の取得
	float RealTimer::GetProgress() const {
		float progress = duration_.count() / goalDuration_.count();
		return (progress < 1.f) ? progress : 1.f;
	}

	bool FunctionTimer::Update() {
		bool isActive = Timer::Update(); // 実行中であるか
		if (isActive && IsFinish())	// 実行中である & ゴールに到達している => 実行
			function_();
		return isActive;
	}
}