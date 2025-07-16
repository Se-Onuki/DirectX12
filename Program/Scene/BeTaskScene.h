/// @file BeTaskScene.h
/// @brief バックエンドのタスクシーン
/// @author ONUKI seiya
#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../Engine/ECS/World/NewWorld.h"
#include "../Engine/ECS/System/FunctionalSystem.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/Particle.h"

#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"
#include "../Engine/UI/Text/NumberRender.h"
#include <future>

class BeTaskScene : public SolEngine::IScene {
public:

	enum class TaskState {
		kStart,		// 実行中
		kStop,		// 停止
		kSendScore,	// スコア送信
	};

	BeTaskScene();
	~BeTaskScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:

	/// @brief タイマーの場所を設定
	void SetTimerPositon();

	/// @brief 与えられた時間をテキストに変換
	/// @param time[in] 時間 
	void TimeToText(const std::chrono::milliseconds &time);

	/// @brief 時間のテキストの描画
	void DrawTimerText() const;

	/// @brief 時間からスコアを算出
	/// @param time[in] 時間
	/// @return 算出したスコア
	uint32_t TimeToScore(const std::chrono::milliseconds &time) const;

	/// @brief スコアを送信する
	void SendScore();

	/// @brief 最高得点を取得
	void GetTopScore();

	/// @brief スコアの非同期送信
	/// @param score[in] 送信するスコア
	/// @return 送信結果を含む std::string を格納した std::future オブジェクト
	static std::future<std::string> PostScoreAsync(int32_t score);

	/// @brief すべてのスコアを非同期的に取得します。
	/// @return すべてのスコアを含む std::string を格納した std::future オブジェクト
	static std::future<std::string> GetAllScoreAsync();

	/// @brief ログイン処理
	/// @param name[in] 名前
	/// @param password[in] パスワード 
	/// @return 結果
	static std::future<std::string> LoginAsync(const std::string &name, const std::string &password);

private:
	// 入力インスタンス
	SolEngine::Input *input_ = nullptr;
	// 音インスタンス
	SolEngine::Audio *audio_ = nullptr;
	// カメラマネージャー
	SolEngine::CameraManager *cameraManager_ = nullptr;
	// 指示文のテキスト
	std::unique_ptr<Sprite> text_;

	// 押すボタン
	std::unique_ptr<Sprite> button_;

	std::array<std::unique_ptr<SolEngine::NumberText>, 2> number_;

	std::unique_ptr<SolEngine::NumberText> scoreText_;

	std::array<std::unique_ptr<SolEngine::NumberText>, 5> topScoreText_;

	// 高精度クロックの開始時刻
	std::chrono::steady_clock::time_point startTime_;

	// 処理時間
	std::chrono::milliseconds timeDuration_;

	const SoLib::Time::SecondF drawTimeCount_ = 7;

	// bgm
	SolEngine::Audio::SoundHandle soundA_;

	TaskState state_ = TaskState::kStop;
};
