#pragma once
#include <functional>

/// @brief タイマークラス
class Timer {
	uint32_t goalFlame_;
	uint32_t nowFlame_;
	bool isFinish_;

	/// @brief フレームの加算
	void AddFlame();

	/// @brief 終点フレームの設定
	/// @param GoalFlame 終点フレーム
	inline void SetGoal(const int &Goal) {
		goalFlame_ = Goal;
	}

public:
	Timer(uint32_t goal = 0u) : goalFlame_(goal), nowFlame_(0), isFinish_(true) {}

	/// @brief 更新処理
	/// @return bool 実行中である場合true
	virtual bool Update();

	/// @brief フレームのリセット
	void Reset();

	/// @brief フレームの初期化
	/// @param goal 必要フレーム
	void Init(uint32_t goal = 0u);

	/// @brief フレームが終点に到達しているか
	/// @return bool ゴールの場合true
	inline const bool &IsFinish() const {
		return isFinish_;
	}

	/// @brief 現在フレームを取得
	/// @return uint32_t 現在フレーム
	inline const uint32_t &GetNowFlame() const {
		return nowFlame_;
	}

	/// @brief ゴールフレームを取得
	/// @return uint32_t ゴールフレーム
	inline const uint32_t &GetGoalFlame() const {
		return goalFlame_;
	}

	/// @brief 現在進行度を取得
	/// @return float 0.0f ~ 1.0fの現在進行度
	float GetProgress() const;
};

class FunctionTimer : public Timer {
	std::function<void(void)> function_ = nullptr;

public:
	FunctionTimer(const std::function<void(void)> &func, uint32_t goal = 0u) : function_(func) {
		Init(goal);
	}
	bool Update() override;
};