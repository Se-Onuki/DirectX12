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

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/Particle.h"

#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"
#include "../Game/Resource/GameScore.h"
#include "../Header/Object/Fade.h"

class ResultScene : public SolEngine::IScene {
public:
	ResultScene();
	~ResultScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

	void SetGameScore(const GameScore &gameScore);

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

	//std::unique_ptr<Sprite> sprite_;
	//std::unique_ptr<Sprite> button_;

	Ground ground_{ 0.f, Vector2{100.f,100.f} };

	BlockManager *blockRender_;

	SolEngine::Camera3D camera_;

	// bgm
	SolEngine::Audio::SoundHandle soundA_;
};