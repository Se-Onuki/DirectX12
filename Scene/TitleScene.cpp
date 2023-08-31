#include "TitleScene.h"

#include "../externals/imgui/imgui.h"
#include "GameScene.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "../DirectBase/2D/Sprite.h"
#include "../Header/Model/Model.h"

#include "../DirectBase/Input/Input.h"
#include "../DirectBase/Base/Audio.h"

TitleScene::TitleScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter()
{
	title_.reset(Sprite::Create(TextureManager::Load("UI/title.png"), MiddleCentor, TopRight));
	title_->SetPivot({ 0.5f,0.5f });

	/*plane_.reset(Sprite::Create(TextureManager::Load("debugfont.png"), MiddleCentor, Vector2{ 200.f,200.f }));
	plane_->SetPivot({ 0.5f,0.5f });*/

	light_.reset(DirectionLight::Create());
}

void TitleScene::OnExit() {
}

void TitleScene::Update() {

	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {
		sceneManager_->ChangeScene(new GameScene, 60);
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

	// モデルの描画

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	title_->Draw();

	// plane_->Draw();

	Sprite::EndDraw();

#pragma endregion

}
