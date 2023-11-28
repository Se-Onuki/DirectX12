#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/LevelElementManager.h"
#include "../Header/Object/Particle.h"
#include "../Header/Object/Particle/ParticleManager.h"
#include "../Header/Object/Particle/ParticleEmitterManager.h"

#include "../Header/Object/TitleManager/TitleManager.h"

class TitleScene : public IScene {
public:
	TitleScene();
	~TitleScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:
	// 入力インスタンス
	Input *input_ = nullptr;
	// 音インスタンス
	Audio *audio_ = nullptr;
	// カメラマネージャー
	CameraManager* cameraManager_ = nullptr;

	// パーティクルマネージャ
	ParticleManager* particleManager_ = nullptr;
	// パーティクル発生装置マネージャ
	ParticleEmitterManager* emitterManager_ = nullptr;

	// 平行光源
	std::unique_ptr<DirectionLight> light_ = nullptr;

	// タイトルマネージャー
	std::unique_ptr<TitleManager> titleManager_ = nullptr;

	// シーン遷移中トリガー
	bool sceneChanging_ = false;

	// BGM関係
	static uint32_t titleSceneBGM_;
	int voiceTitleSceneBGMHandle_ = 0u;

	// SE関係
	static uint32_t selectSE_; // 開始SE
};