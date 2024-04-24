#include "SoLib_Timer.h"
#include <algorithm>

namespace SoLib {
	bool FlameTimer::Update() {
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

	void FlameTimer::AddFlame() {
		nowFlame_++;
	}

	void FlameTimer::Start() {
		nowFlame_ = 0u;
		isFinish_ = false;
		isActive_ = true;
	}

	void FlameTimer::Start(uint32_t goal) {
		Start();
		SetGoal(goal);
	}

	void FlameTimer::Clear() {
		nowFlame_ = 0u;
		isFinish_ = true;
		isActive_ = false;
		goalFlame_ = 0u;
	}

	float FlameTimer::GetProgress() const {
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




	bool DeltaTimer::Update(float deltaTime) {
		// 終了フラグが立っていたら
		if (isFinish_) {
			// ループが有効なら
			if (isLoop_) {
				// 終了フラグを折る
				isFinish_ = false;
			}
			// ループが無効なら
			else {
				// 動作を終了する｡
				isActive_ = false;
				return false;
			}
		}
		// 値を加算する
		AddFlame(deltaTime);
		if (nowFlame_ >= goalFlame_) {
			isFinish_ = true;
			// もしループが有効であったら
			if (isLoop_) {
				// ゴール以降の時間を返す
				nowFlame_ = std::fmodf(nowFlame_, goalFlame_);
			}
		}
		return true;
	}

	void DeltaTimer::AddFlame(float deltaTime) {
		nowFlame_ += deltaTime;
	}

	void DeltaTimer::Start(bool isLoop) {
		nowFlame_ = 0.f;
		isFinish_ = false;
		isActive_ = true;
		isLoop_ = isLoop;
	}

	void DeltaTimer::Start(float goal, bool isLoop) {
		Start(isLoop);
		SetGoal(goal);
	}

	void DeltaTimer::Clear() {
		nowFlame_ = 0u;
		isFinish_ = true;
		isActive_ = false;
		goalFlame_ = 0u;
	}

	float DeltaTimer::GetProgress() const {
		// もしゴールが未設定の場合は0を返す
		if (not goalFlame_) { return 0.f; }
		return std::clamp(nowFlame_ / goalFlame_, 0.f, 1.f);
	}

	bool FunctionTimer::Update() {
		bool isActive = FlameTimer::Update(); // 実行中であるか
		if (isActive && IsFinish())	// 実行中である & ゴールに到達している => 実行
			function_();
		return isActive;
	}
}