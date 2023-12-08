#include "TitleScene.h"

#include <imgui.h>
#include "StageSelectScene.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Utils/SoLib/SoLib.h"

#include "../Header/Object/Fade.h"

// 静的なメンバ変数の実体を宣言
uint32_t TitleScene::titleSceneBGM_ = 0u;
uint32_t TitleScene::selectSE_ = 0u;

TitleScene::TitleScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter() {
	// ライトの生成
	light_.reset(DirectionLight::Create());

	// インスタンス取得
	static auto *const modelManager = ModelManager::GetInstance();
	particleManager_ = ParticleManager::GetInstance();		 // パーティクルマネージャ
	emitterManager_ = ParticleEmitterManager::GetInstance(); // 発生マネージャ

	// カメラマネージャーの初期化
	cameraManager_->Init();
	cameraManager_->GetUseCamera()->translation_ = { .15f, -1.25f, -100.0f };
	cameraManager_->GetUseCamera()->rotation_ = { -.25f, 0.0f, 0.0f };

	// パーティクル関連の初期化
	particleManager_->Init(256); // パーティクルの最大数は256
	emitterManager_->Init();	 // 発生マネージャの初期化

	titleManager_ = std::make_unique<TitleManager>();
	titleManager_->Initialize();

	sceneChanging_ = false;

	// SE読み込み
	if (titleSceneBGM_ == 0u) {
		titleSceneBGM_ = audio_->LoadWave("resources/Audio/BGM/TitleBGM.wav");
	}
	if (selectSE_ == 0u) {
		selectSE_ = audio_->LoadWave("resources/Audio/SE/UI/select.wav");
	}

	// フェードイン開始
	Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 0.0f }, 2.5f);
}

void TitleScene::OnExit() {

	StageSelectManager::GetInstance()->Reset();
	audio_->StopAllWave();
}

void TitleScene::Update() {

	// キーボードの入力取得
	static const auto *const keyBoard = input_->GetDirectInput();

	// デルタタイムの取得
	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	// BGM再生
	if (audio_->IsPlaying(voiceTitleSceneBGMHandle_) == 0 || voiceTitleSceneBGMHandle_ == -1) {
		voiceTitleSceneBGMHandle_ = audio_->PlayWave(titleSceneBGM_, false, BGMVolume_);
	}

	// パーティクルの更新
	emitterManager_->Update(deltaTime);  // 発生マネージャ
	particleManager_->Update(deltaTime); // パーティクル

	// タイトル画面の更新
	titleManager_->Update(deltaTime);

	// カメラマネージャーの更新
	cameraManager_->Update(deltaTime);

#ifdef _DEBUG // デバッグ時のみImGuiを描画
	// カメラマネージャーのImGuiを表示
	cameraManager_->DisplayImGui();
#endif // _DEBUG // デバッグ時のみImGuiを描画

	// スペースを押すと次のシーンへ
	if (keyBoard->IsTrigger(DIK_SPACE) || input_->GetXInput()->IsTrigger(KeyCode::A)) {
		if (!sceneChanging_) {
			// 決定SEを再生
			audio_->PlayWave(selectSE_, false, 0.45f);

			// フェードアウト開始
			Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
			// 指定した秒数後シーンチェンジ
			sceneManager_->ChangeScene(std::make_unique<StageSelectScene>(), 1.0f);
			sceneChanging_ = true;
		}
	}

}

void TitleScene::Draw() {
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

	Model::SetPipelineType(Model::PipelineType::kModel);

	const auto &camera = *cameraManager_->GetUseCamera();

	titleManager_->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kParticle);
	static auto *const particleManager = ParticleManager::GetInstance();

	// 複数モデルのパーティクルを、それぞれの集合ごとに描画
	particleManager->Draw(camera);

	// モデルの描画


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	/*sprite_->Draw();*/

	titleManager_->SpriteDraw();

	// フェード演出描画
	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}
