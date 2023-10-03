#include "GameScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"

#include "TitleScene.h"

#include "../Header/Entity/Component/ModelComp.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	light_.reset(DirectionLight::Create());

	model_ = ModelManager::GetInstance()->AddModel("sphere", Model::LoadObjFile("", "sphere.obj"));
	transform_.UpdateMatrix();
	camera_.Init();

	sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });
}

void GameScene::OnExit() {}

void GameScene::Update() {

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();
	camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	model_->ImGuiWidget();
	transform_.ImGuiWidget();
	ImGui::End();

	transform_.UpdateMatrix();
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
	model_->Draw(transform_, camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}