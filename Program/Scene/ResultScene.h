/// @file ResultScene.h
/// @brief リザルトシーン
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
#include "../Game/Resource/GameScore.h"
#include "../Header/Object/Fade.h"
#include "../Engine/UI/Text/NumberRender.h"

class ResultScene : public SolEngine::IScene {
public:
	ResultScene();
	~ResultScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

	void SetGameScore(const GameScore &gameScore);

	void SetTimerPos(Vector2 pos);

private:

	// ゲームスコア
	GameScore gameScore_;
	// フェード
	Fade *fade_ = nullptr;
	// 入力インスタンス
	SolEngine::Input *input_ = nullptr;
	// 音インスタンス
	SolEngine::Audio *audio_ = nullptr;
	// カメラマネージャー
	SolEngine::CameraManager *cameraManager_ = nullptr;

	std::unique_ptr<Sprite> resultText_;
	//std::unique_ptr<Sprite> button_;

	ECS::World world_;
	ECS::SystemExecuter systemExecuter_;

	// 平行光源
	std::unique_ptr<DirectionLight> light_ = nullptr;

	Ground ground_{ 0.f, Vector2{100.f,100.f} };

	BlockManager *blockRender_;
	ModelHandleListManager *blockHandleRender_;

	SolEngine::Camera3D camera_;

	// bgm
	SolEngine::Audio::SoundHandle soundA_;

	// 時間表示のUI
	std::array<std::unique_ptr<SolEngine::NumberText>, 2u> gameTimerUI_;

	std::unique_ptr<Sprite> timerCoron_;
};