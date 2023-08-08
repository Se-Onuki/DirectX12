#include "GameScene.h"
#include "../externals/imgui/imgui.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "TitleScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::OnEnter()
{

	light_.reset(DirectionLight::Create());

}

void GameScene::OnExit()
{
}

void GameScene::Update()
{

}

void GameScene::Draw()
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

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}
