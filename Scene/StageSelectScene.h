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
#include "../Header/Object/StageSelectManager/StageSelectManager.h"

/// <summary>
/// ステージ選択シーン
/// </summary>
class StageSelectScene : public IScene {
public: // コンストラクタ等

	StageSelectScene();
	~StageSelectScene();

public: // メンバ関数

	// シーン入場時に呼び出される関数
	void OnEnter() override;
	// シーン退出時
	void OnExit() override;

	// 更新関数
	void Update() override;
	// 描画関数
	void Draw() override;

private: // メンバ変数

	// 入力インスタンス
	Input* input_ = nullptr;
	// 音インスタンス
	Audio* audio_ = nullptr;
	// カメラマネージャー
	CameraManager* cameraManager_ = nullptr;

	// パーティクルマネージャ
	ParticleManager* particleManager_ = nullptr;
	// パーティクル発生装置マネージャ
	ParticleEmitterManager* emitterManager_ = nullptr;

	// ステージ選択マネージャ
	StageSelectManager* stageSelectManager_ = nullptr;

	// 平行光源
	std::unique_ptr<DirectionLight> light_ = nullptr;

	// シーン遷移中トリガー
	bool sceneChanging_ = false;

	// BGM関係
	float BGMVolume_ = 0.35f;
	static uint32_t stageSelectSceneBGM_;
	int voiceStageSelectSceneBGMHandle_ = 0u;

	// 効果音系
	static uint32_t startStageSE_; // ステージ開始SE

};

