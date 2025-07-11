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

class BeTaskScene : public SolEngine::IScene {
public:
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

	// 高精度クロックの開始時刻
	std::chrono::steady_clock::time_point startTime_;

	// 処理時間
	std::chrono::milliseconds timeDuration_;

	// bgm
	SolEngine::Audio::SoundHandle soundA_;

	bool isTimerStart_ = false;
};
