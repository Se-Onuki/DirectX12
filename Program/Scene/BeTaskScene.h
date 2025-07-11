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

	void SetTimerPositon();

	void TimeToText(const std::chrono::milliseconds &time);

	void DrawTimerText() const;

	uint32_t TimeToScore(const std::chrono::milliseconds &time) const;

	void SendScore();

	void GetTopScore();

	static std::future<std::string> PostScoreAsync(int32_t score);

	static std::future<std::string> GetAllScoreAsync();

	static size_t WriteCallback(void *const c, const size_t s, const size_t n, std::string *const userp);


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

	SoLib::Time::SecondF drawTimeCount_ = 7;

	// bgm
	SolEngine::Audio::SoundHandle soundA_;

	TaskState state_ = TaskState::kStop;
};
