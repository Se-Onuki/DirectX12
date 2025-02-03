/// @file TitleScene.h
/// @brief タイトルシーン
/// @author ONUKI seiya
#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/Particle.h"

#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"

class TitleScene : public SolEngine::IScene {
public:
	TitleScene();
	~TitleScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:
	// 入力インスタンス
	SolEngine::Input *input_ = nullptr;
	// 音インスタンス
	SolEngine::Audio *audio_ = nullptr;
	// カメラマネージャー
	CameraManager *cameraManager_ = nullptr;

	// 平行光源
	std::unique_ptr<DirectionLight> light_ = nullptr;

	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> button_;

	Ground ground_{ 0.f, Vector2{100.f,100.f} };

	BlockManager *blockRender_;

	SolEngine::Camera3D camera_;

	// bgm
	SolEngine::Audio::SoundHandle soundA_;
};
