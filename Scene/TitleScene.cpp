#include "TitleScene.h"

#include "../externals/imgui/imgui.h"
#include "GameScene.h"
#include "../DirectBase/Base/DirectXCommon.h"


TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::OnEnter()
{
	light_.reset(DirectionLight::Create());
}

void TitleScene::OnExit()
{
}

void TitleScene::Update()
{

	ImGui::Begin("SceneChanger");
	if (ImGui::Button("Reload : Delay 30Flame")) {
		SceneManager::GetInstance()->ChangeScene(new TitleScene, 30);
	}
	if (ImGui::Button("GameScene : Delay 30Flame")) {
		SceneManager::GetInstance()->ChangeScene(new GameScene, 30);
	}
	ImGui::End();

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

	Sprite::EndDraw();

#pragma endregion

}
