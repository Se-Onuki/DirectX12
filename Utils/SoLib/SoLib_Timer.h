#pragma once
#include <functional>
#include <chrono>


namespace SoLib {
	/// @brief タイマークラス
	class Timer {
		uint32_t goalFlame_;
		uint32_t nowFlame_;
		bool isFinish_;
		bool isActive_;

		/// @brief フレームの加算
		void AddFlame();

		/// @brief 終点フレームの設定
		/// @param GoalFlame 終点フレーム
		inline void SetGoal(const int &Goal) { goalFlame_ = Goal; }

	public:
		Timer(uint32_t goal = 0u) : goalFlame_(goal), nowFlame_(0), isFinish_(true), isActive_(false) {}

		/// @brief 更新処理 ( 基本的に各フレームの先頭で行うこと )
		/// @return bool 実行中である場合true
		virtual bool Update();

		/// @brief フレームのリセット
		void Start();

		/// @brief フレームの初期化
		/// @param goal 必要フレーム
		void Start(uint32_t goal);

		/// @brief 完全停止
		void Clear();

		/// @brief フレームが終点に到達しているか
		/// @return bool ゴールの場合true
		inline  bool IsFinish() const { return isFinish_; }

		/// @brief 実行中であるかどうか
		/// @return bool 実行中である場合 true
		bool IsActive() const { return isActive_; }

		/// @brief 現在フレームを取得
		/// @return uint32_t 現在フレーム
		inline  uint32_t GetNowFlame() const { return nowFlame_; }

		/// @brief ゴールフレームを取得
		/// @return uint32_t ゴールフレーム
		inline  uint32_t GetGoalFlame() const { return goalFlame_; }

		/// @brief 現在進行度を取得
		/// @return float 0.0f ~ 1.0fの現在進行度
		float GetProgress() const;
	};

	class RealTimer {
		std::chrono::steady_clock::time_point startTime_;
		std::chrono::duration<float> duration_;
		std::chrono::duration<float> goalDuration_;
		bool isFinish_;
		bool isActive_;

	public:
		RealTimer() = default;

		bool Update();

		void Start();

		void Start(float seconds);

		void Clear();

		bool IsFinish() const { return isFinish_; }

		bool IsActive() const { return isActive_; }

		float GetElapsedSeconds() const { return duration_.count(); }

		float GetGoalSeconds() const { return goalDuration_.count(); }

		float GetProgress() const;
	};


	class FunctionTimer : public Timer {
		std::function<void(void)> function_ = nullptr;

	public:
		FunctionTimer(const std::function<void(void)> &func, uint32_t goal = 0u) : function_(func) {
			Start(goal);
		}
		bool Update() override;
	};
}