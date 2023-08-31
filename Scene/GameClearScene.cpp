#include "GameClearScene.h"

#include <imgui.h>
#include "TitleScene.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "../DirectBase/2D/Sprite.h"
#include "../Header/Model/Model.h"

#include "../DirectBase/Input/Input.h"
#include "../DirectBase/Base/Audio.h"

GameClearScene::GameClearScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
}

GameClearScene::~GameClearScene() {
}

void GameClearScene::OnEnter()
{
	gameClearSprite_.reset(Sprite::Create(TextureManager::Load("UI/title.png"), MiddleCentor, TopRight));
	gameClearSprite_->SetPivot({ 0.5f,0.5f });
}

void GameClearScene::OnExit() {
}

void GameClearScene::Update() {

	sceneManager_->ChangeScene(new TitleScene, 60);
}

void GameClearScene::Draw() {
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

	gameClearSprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}
