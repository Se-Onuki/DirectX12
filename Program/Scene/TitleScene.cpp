#include "TitleScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

#include "../Header/Object/Fade.h"
#include "GameScene.h"

TitleScene::TitleScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
	blockRender_ = BlockManager::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter() {
	// ライトの生成
	light_ = DirectionLight::Create();
	blockRender_->Init(1024u);
	ModelManager::GetInstance()->CreateDefaultModel();

	sprite_ = Sprite::Create(TextureManager::Load("UI/TitleECS.png"));
	button_ = Sprite::Create(TextureManager::Load("UI/TD2_3week_2/Title/Start_CR.png"));

	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);

	ground_.Init();
	camera_.Init();

	camera_.translation_.y = 5.f;

	// bgmのロード
	soundA_ = audio_->LoadMP3("resources/Audio/BGM/TitleBGM.mp3");

	soundA_.Play(true, 0.5f);

}

void TitleScene::OnExit() {
	audio_->StopAllWave();
}

void TitleScene::Update() {

	// キーボードの入力取得
	[[maybe_unused]] static const auto *const keyBoard = input_->GetDirectInput();

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	blockRender_->clear();

	camera_.rotation_ *= SoLib::MakeQuaternion(SoLib::Euler{ 0.f, 22.5_deg * deltaTime, 0.f });
	camera_.UpdateMatrix();

	sprite_->SetScale(Vector2{ 256,64 } *2.f);
	sprite_->SetPivot({ 0.5f,0.5f });

	sprite_->SetPosition(Vector2{ WinApp::kWindowWidth * 0.5f,WinApp::kWindowHeight * (1.f / 4.f) });

	button_->SetScale(Vector2{ 256,64 } *2.f);
	button_->SetPivot({ 0.5f,0.5f });

	button_->SetPosition(Vector2{ WinApp::kWindowWidth * 0.5f,WinApp::kWindowHeight * (2.f / 4.f) });

	if (input_->GetXInput()->IsTrigger(KeyCode::A) or input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		sceneManager_->ChangeScene<GameScene>(1.f);
		Fade::GetInstance()->Start(Vector2{}, 0x000000FF, 1.f);
	}
	// デルタタイムの取得
	// const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
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
	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);
	ground_.Draw();

	blockRender_->Draw(camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();
	button_->Draw();

	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}
