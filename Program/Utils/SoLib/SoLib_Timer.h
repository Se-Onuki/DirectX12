#pragma once
#include <functional>
#include <chrono>
#include "SoLib_Traits.h"
#include "SoLib_ImGui.h"


namespace SoLib {


	namespace Time {


		template <SoLib::IsFloatPoint T>
		class Second final {
		public:
			Second(T time = 0.f) { time_ = time; }
			Second(const Second &) = default;
			Second &operator=(const Second &) = default;

			template <SoLib::IsFloatPoint U>
			Second(const Second<U> &other) { time_ = static_cast<T>(other.GetTime()); }
			template <SoLib::IsFloatPoint U>
			Second &operator=(const Second<U> &other) {
				time_ = static_cast<T>(other.GetTime());
				return *this;
			}

			using type = T;

			T &GetTime()  noexcept { return time_; }
			const T &GetTime() const noexcept { return time_; }

			inline operator T &()  noexcept { return time_; }
			inline operator const T &() const noexcept { return time_; }

			inline operator std::chrono::duration<T>() const noexcept { return static_cast<std::chrono::duration<T>>(time_); };

		private:
			T time_;
		};

		using SecondF = Second<float>;

		/// @brief タイマークラス
		class FlameTimer {
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
			FlameTimer(uint32_t goal = 0u) : goalFlame_(goal), nowFlame_(0), isFinish_(false), isActive_(false) {}

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

		/// @brief タイマークラス
		class DeltaTimer {
			float goalFlame_;
			float nowFlame_;
			bool isFinish_;
			bool isActive_;

			/// @brief 時間の加算
			void AddFlame(float deltaTime);

			/// @brief 終点フレームの設定
			/// @param GoalFlame 終点フレーム
			inline void SetGoal(float Goal) { goalFlame_ = Goal; }

		public:
			DeltaTimer(float goal = 0.f, bool start = false) : goalFlame_(goal), nowFlame_(0.f), isFinish_(false), isActive_(false) {
				if (start) {
					Start(goal);
				}
			}

			bool ImGuiWidget(const char *const label);

			/// @brief 更新処理 ( 基本的に各フレームの先頭で行うこと )
			/// @return bool 実行中である場合true
			bool Update(float deltaTime);

			/// @brief フレームのリセット
			void Start();

			/// @brief フレームの初期化
			/// @param goal 必要フレーム
			void Start(float goal);

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
			inline  float GetNowFlame() const { return nowFlame_; }

			/// @brief ゴールフレームを取得
			/// @return uint32_t ゴールフレーム
			inline  float GetGoalFlame() const { return goalFlame_; }

			/// @brief 現在進行度を取得
			/// @return float 0.0f ~ 1.0fの現在進行度
			float GetProgress() const;
		};


		class FunctionTimer : public FlameTimer {
			std::function<void(void)> function_ = nullptr;

		public:
			FunctionTimer(const std::function<void(void)> &func, uint32_t goal = 0u) : function_(func) {
				Start(goal);
			}
			bool Update() override;
		};
	}

	using namespace Time;
}

inline bool SoLib::DeltaTimer::ImGuiWidget(const char *const label) {

#ifdef USE_IMGUI
	bool isChanged = false;

	isChanged |= ImGui::DragFloat(label, &this->goalFlame_, 0.1f, 0.f, 100.f, "%.3fsec");

	if (ImGui::Button("Stop")) {
		this->Clear();
		isChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Start")) {
		this->Start();
		isChanged = true;
	}

	return isChanged;

#else

	label;
	return false;

#endif // USE_IMGUI

	}