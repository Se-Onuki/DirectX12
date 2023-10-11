#include "GameScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"

#include "TitleScene.h"

#include "../Header/Entity/Component/ModelComp.h"
#include "../Utils/SoLib/SoLib_ImGui.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	light_.reset(DirectionLight::Create());

	// model_ = ModelManager::GetInstance()->AddModel("Fence", Model::LoadObjFile("Model/Fence/", "fence.obj"));
	transform_.UpdateMatrix();
	camera_.Init();

	sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });


	Model *const playerBody =
		ModelManager::GetInstance()->AddModel("playerBody", Model::LoadObjFile("Model/Player/", "body.obj"));
	Model *const playerHead =
		ModelManager::GetInstance()->AddModel("playerHead", Model::LoadObjFile("Model/Player/", "head.obj"));
	Model *const playerLeft =
		ModelManager::GetInstance()->AddModel("playerLeft", Model::LoadObjFile("Model/Player/", "left.obj"));
	Model *const playerRight =
		ModelManager::GetInstance()->AddModel("playerRight", Model::LoadObjFile("Model/Player/", "right.obj"));
	Model *const playerWeapon =
		ModelManager::GetInstance()->AddModel("playerWeapon", Model::LoadObjFile("Model/Player/", "hammer.obj"));

	/*Model *const enemyBody =
		ModelManager::GetInstance()->AddModel("enemyBody", Model::LoadObjFile("needle_Body"));*/

	std::unordered_map<std::string, Model *> playerMap_{
		{"body",   playerBody  },
		{"head",   playerHead  },
		{"right",  playerRight },
		{"left",   playerLeft  },
		{"weapon", playerWeapon},
	};

	player_.reset(new Player{});
	player_->Init(playerMap_);

	followCamera_.reset(new FollowCamera);
	followCamera_->Init();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetViewProjection(followCamera_->GetCamera());


}

void GameScene::OnExit() {}

void GameScene::Update() {

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();

	//camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	//model_->ImGuiWidget();
	transform_.ImGuiWidget();
	ImGui::End();

	TextureManager::GetInstance()->ImGuiWindow();

	light_->ImGuiWidget();

	player_->Update();

	followCamera_->Update();
	camera_ = *followCamera_->GetCamera();

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
	//model_->Draw(transform_, camera_);
	player_->Draw(camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}