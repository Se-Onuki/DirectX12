#include "GameOverScene.h"

#include <imgui.h>
#include "TitleScene.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "../DirectBase/2D/Sprite.h"
#include "../Header/Model/Model.h"

#include "../DirectBase/Input/Input.h"
#include "../DirectBase/Base/Audio.h"

GameOverScene::GameOverScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
}

GameOverScene::~GameOverScene() {
}

void GameOverScene::OnEnter()
{
	backGround_.reset(Sprite::Create());
	backGround_->SetPosition(MiddleCentor);
	backGround_->SetPivot({ 0.5f,0.5f });
	backGround_->SetScale(TopRight);

	gameOverSprite_.reset(Sprite::Create(TextureManager::Load("UI/gameOver.png"), MiddleCentor, TopRight));
	gameOverSprite_->SetPivot({ 0.5f,0.5f });
}

void GameOverScene::OnExit() {
}

void GameOverScene::Update() {

	sceneManager_->ChangeScene(new TitleScene, 120);
}

void GameOverScene::Draw() {
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

	// モデルの描画

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);
	backGround_->Draw();

	gameOverSprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}
