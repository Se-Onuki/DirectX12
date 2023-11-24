#include "StageSelectScene.h"
#include "GameScene.h"

#include "../Header/Object/Fade.h"

StageSelectScene::StageSelectScene()
{
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
}

StageSelectScene::~StageSelectScene(){}

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

	// フェードイン開始
	Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 0.0f }, 1.0f);
}

void StageSelectScene::OnExit()
{
	
}

void StageSelectScene::Update()
{

	// キーボードの入力取得
	static const auto* const keyBoard = input_->GetDirectInput();

	// デルタタイムの取得
	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	// パーティクルの更新
	emitterManager_->Update(deltaTime);  // 発生マネージャ
	particleManager_->Update(deltaTime); // パーティクル

	// ステージ選択マネージャの更新
	stageSelectManager_->Update(deltaTime);
	stageSelectManager_->DisplayImGui();

	// カメラマネージャーの更新
	cameraManager_->Update(deltaTime);

	// スペースを押すと次のシーンへ
	if (keyBoard->IsTrigger(DIK_SPACE)) {

		// フェードアウト開始
		Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);

		// モデルロードが終わり次第シーンを離れる
		sceneManager_->ChangeScene(std::make_unique<GameScene>(), 1.0f);
	}
}

void StageSelectScene::Draw()
{
	DirectXCommon* const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* const commandList = dxCommon->GetCommandList();

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
	static auto* const particleManager = ParticleManager::GetInstance();

	// 複数モデルのパーティクルを、それぞれの集合ごとに描画
	particleManager->Draw(*cameraManager_->GetUseCamera());

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
