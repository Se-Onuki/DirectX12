/// @file SoLib_Timer.h
/// @brief タイマー
/// @author ONUKI seiya
#pragma once
#include <functional>
#include <chrono>
#include "SoLib_Traits.h"
#include "SoLib_ImGui.h"


namespace SoLib {


	namespace Time {

		class DeltaTimer;
	}
	template<>
	bool ImGuiWidget(const char *const label, Time::DeltaTimer *const ptr);
	namespace Time {

		/// @brief 与えられた関数から､処理の実行時間を計測する
		template <typename T>
		T MeasureTime(const std::function<void(void)> &func) {
			// 高精度クロックの開始
			auto start = std::chrono::high_resolution_clock::now();

			func();

			// 高精度クロックの終了
			auto end = std::chrono::high_resolution_clock::now();

			// 処理時間を計算
			auto duration = std::chrono::duration_cast<T>(end - start);

			return duration;
		}


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
			/// @brief 時間の取得
			T &GetTime()  noexcept { return time_; }
			const T &GetTime() const noexcept { return time_; }

			inline operator T &()  noexcept { return time_; }
			inline operator const T &() const noexcept { return time_; }

			inline operator std::chrono::duration<T>() const noexcept { return static_cast<std::chrono::duration<T>>(time_); };

		private:
			T time_;
		};

		using SecondF = Second<float>;

		/// @brief 秒数から時間を算出する
		/// @param[in] time 秒数
		/// @return 換算した時間(分, 秒)
		inline std::pair<uint32_t, uint32_t> GetMoment(const float time) {
			const uint32_t iTime = static_cast<uint32_t>(time);
			std::pair<uint32_t, uint32_t> result;

			result.first = iTime / 60;
			result.second = iTime % 60;

			return result;
		}

		/// @brief ミリ秒から時間を算出する
		/// @param[in] time ミリ秒
		/// @return 換算した時間(秒, 小数点以下)
		inline std::pair<uint32_t, uint32_t> GetMilliMoment(const std::chrono::milliseconds &time) {
			auto millisec = time.count();
			std::pair<uint32_t, uint32_t> result;
			result.first = static_cast<uint32_t>(millisec / 1000);
			result.second = static_cast<uint32_t>(millisec % 1000);
			return result;
		}

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
			inline void SetGoal(const int32_t Goal) { goalFlame_ = Goal; }

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
			/// @brief 更新処理
			bool Update();
			/// @brief フレームのリセット
			void Start();
			/// @brief フレームの初期化
			/// @param goal 必要フレーム
			void Start(float seconds);
			/// @brief 完全停止
			void Clear();
			/// @brief フレームが終点に到達しているか
			bool IsFinish() const { return isFinish_; }
			/// @brief 実行中であるかどうか
			bool IsActive() const { return isActive_; }
			/// @brief 現在フレームを取得
			float GetElapsedSeconds() const { return duration_.count(); }
			/// @brief ゴールフレームを取得
			float GetGoalSeconds() const { return goalDuration_.count(); }
			/// @brief 現在進行度を取得
			float GetProgress() const;
		};

		/// @brief タイマークラス
		class DeltaTimer {
			float goalFlame_;
			float nowFlame_;
			bool isFinish_;
			bool isActive_;
			bool isLoop_ = false;

			/// @brief 時間の加算
			void AddFlame(float deltaTime);

			/// @brief 終点フレームの設定
			/// @param GoalFlame 終点フレーム
			inline void SetGoal(float Goal) { goalFlame_ = Goal; }

		public:
			friend bool ::SoLib::ImGuiWidget(const char *const label, DeltaTimer *const ptr);
			DeltaTimer(float goal = 0.f, bool start = true, bool isLoop = false) : goalFlame_(goal), nowFlame_(0.f), isFinish_(false), isActive_(false), isLoop_(isLoop) {
				if (start) {
					Start(goal, isLoop);
				}
			}

			void SetNowTime(float time) { nowFlame_ = time; }

			/// @brief 更新処理 ( 基本的に各フレームの先頭で行うこと )
			/// @return bool 実行中である場合true
			bool Update(float deltaTime);

			/// @brief フレームのリセット
			void Start(bool isLoop = false);

			/// @brief フレームの初期化
			/// @param goal 必要フレーム
			/// @param isLoop ループを行うか
			void Start(float goal, bool isLoop = false);

			/// @brief 最初に戻す
			void Reset();

			/// @brief 完全停止
			void Clear();

			/// @brief フレームが終点に到達しているか
			/// @return bool ゴールの場合true
			inline  bool IsFinish() const { return isFinish_; }

			/// @brief 実行中であるかどうか
			/// @return bool 実行中である場合 true
			bool IsActive() const { return isActive_; }

			/// @brief 現在フレームを取得
			/// @return float 現在フレーム
			inline  float GetNowFlame() const { return nowFlame_; }

			/// @brief ゴールフレームを取得
			/// @return float ゴールフレーム
			inline  float GetGoalFlame() const { return goalFlame_; }

			/// @brief 残り時間を取得
			/// @return float 残り時間
			inline float GetTimeRemain() const { return goalFlame_ - nowFlame_; }

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

	template<>
	inline bool ImGuiWidget(const char *const label, Time::DeltaTimer *const data) {

#ifdef USE_IMGUI
		bool isChanged = false;

		isChanged |= ImGui::DragFloat(label, &(data->goalFlame_), 0.1f, 0.f, 100.f, "%.3fsec");

		if (ImGui::Button("Stop")) {
			data->Clear();
			isChanged = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Start")) {
			data->Start();
			isChanged = true;
		}

		return isChanged;

#else

		label; data;
		return false;

#endif // USE_IMGUI

	}


	using namespace Time;
}
