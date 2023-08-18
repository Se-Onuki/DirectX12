#include "GameScene.h"
#include "../externals/imgui/imgui.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "TitleScene.h"
#include "../DirectBase/Input/Input.h"

#include "../Header/Player/Player.h"
#include "../Header/Model/ModelManager.h"

GameScene::GameScene() {
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	viewProjection_.Init();
	viewProjection_.translation_ = { 0.f,0.f,-15.f };

	Model *const playerBody =
		ModelManager::GetInstance()->AddModel("playerBody", Model::LoadObjFile("", "sphere.obj"));

	std::unordered_map<std::string, Model *> playerMap_{
		{"body",   playerBody  },
	};

	player_.reset(new Player);
	player_->Init(playerMap_);

	light_.reset(DirectionLight::Create());
}

void GameScene::OnExit()
{
}

void GameScene::Update() {
	const DirectInput *const directInput = DirectInput::GetInstance();
	//const XInput *const xInput = XInput::GetInstance();
	viewProjection_.ImGuiWidget();
	viewProjection_.UpdateMatrix();
	if (directInput->IsPress(DIK_A)) {
		light_->ImGuiWidget();
	}
	player_->Update();
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
	player_->Draw(viewProjection_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}
