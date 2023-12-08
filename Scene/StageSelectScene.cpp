#include "StageSelectScene.h"
#include "GameScene.h"
#include "TitleScene.h"

#include "../Header/Object/Fade.h"

// 静的なメンバ変数の実体を宣言
uint32_t StageSelectScene::stageSelectSceneBGM_ = 0u;
uint32_t StageSelectScene::startStageSE_ = 0u;

StageSelectScene::StageSelectScene()
{
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
}

StageSelectScene::~StageSelectScene() {}

void StageSelectScene::OnEnter()
{
	// ライトの生成
	light_.reset(DirectionLight::Create());

	// インスタンス取得
	particleManager_ = ParticleManager::GetInstance();		 // パーティクルマネージャ
	emitterManager_ = ParticleEmitterManager::GetInstance(); // 発生マネージャ
	stageSelectManager_ = StageSelectManager::GetInstance(); // ステージ選択マネージャ

	// カメラマネージャーの初期化
	cameraManager_->Init();

	// パーティクル関連の初期化
	particleManager_->Init(256); // パーティクルの最大数は256
	emitterManager_->Init();	 // 発生マネージャの初期化

	// ステージ選択マネージャの初期化
	stageSelectManager_->Init();

	sceneChanging_ = false;

	if (stageSelectSceneBGM_ == 0u) {
		stageSelectSceneBGM_ = audio_->LoadWave("resources/Audio/BGM/StageSelectBGM.wav");
	}

	if (startStageSE_ == 0u) {
		startStageSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/select.wav");
	}

	// フェードイン開始
	Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 0.0f }, 1.0f);
}

void StageSelectScene::OnExit()
{
	audio_->StopAllWave();
}

void StageSelectScene::Update()
{

	// キーボードの入力取得
	static const auto *const keyBoard = input_->GetDirectInput();

	// デルタタイムの取得
	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	bgmTimingManager_.Update(deltaTime);
	// BGM再生
	if ((audio_->IsPlaying(voiceStageSelectSceneBGMHandle_) == 0 || voiceStageSelectSceneBGMHandle_ == -1) && bgmTimingManager_.IsFinish()) {
		voiceStageSelectSceneBGMHandle_ = audio_->PlayWave(stageSelectSceneBGM_, false, BGMVolume_);
		bgmTimingManager_.Start(1.f);
	}

	// パーティクルの更新
	emitterManager_->Update(deltaTime);  // 発生マネージャ
	particleManager_->Update(deltaTime); // パーティクル

	// ステージ選択マネージャの更新
	stageSelectManager_->Update(deltaTime);
	stageSelectManager_->DisplayImGui();

	// カメラマネージャーの更新
	cameraManager_->Update(deltaTime);

	// スペースを押すと次のシーンへ
	if (stageSelectManager_->GetImageTimer().IsFinish() || stageSelectManager_->GetImageTimer().GetProgress() > 0.5f) {
		if ((keyBoard->IsPress(DIK_SPACE) || input_->GetXInput()->IsPress(KeyCode::A)) && Fade::GetInstance()->GetSprite()->GetColor().w <= 0.f) {
			if (!sceneChanging_) {
				audio_->PlayWave(startStageSE_, false, 0.45f);
				// フェードアウト開始
				Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
				// 指定した秒数後シーンチェンジ
				sceneManager_->ChangeScene(std::make_unique<GameScene>(), 1.0f);
				sceneChanging_ = true;
			}
		}
	}

	// エスケープを押すとタイトルへ
	if (keyBoard->IsTrigger(DIK_ESCAPE) || input_->GetXInput()->IsTrigger(KeyCode::B)) {
		if (!sceneChanging_) {
			// フェードアウト開始
			Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
			// 指定した秒数後シーンチェンジ
			sceneManager_->ChangeScene(std::make_unique<TitleScene>(), 1.0f);
			sceneChanging_ = true;
		}
	}

}

void StageSelectScene::Draw()
{
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画


	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);

	light_->SetLight(commandList);

	// モデルの描画

	// ステージ選択マネージャモデル描画
	stageSelectManager_->Draw(*cameraManager_->GetUseCamera());

	Model::SetPipelineType(Model::PipelineType::kParticle);
	static auto *const particleManager = ParticleManager::GetInstance();


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// ステージ選択マネージャスプライト描画
	stageSelectManager_->SpriteDraw();

	// フェード演出描画
	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion
}
