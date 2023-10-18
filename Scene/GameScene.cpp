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
	ModelManager::GetInstance()->AddModel("box", Model::LoadObjFile("", "box.obj"));
	transform_.UpdateMatrix();
	camera_.Init();

	sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });

	ModelManager::GetInstance()->AddModel("skydome", Model::LoadObjFile("", "skyCylinder.obj"));

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

	std::unordered_map<std::string, Model *> playerMap{
		{"body",   playerBody  },
		{"head",   playerHead  },
		{"right",  playerRight },
		{"left",   playerLeft  },
		{"weapon", playerWeapon},
	};

	player_.reset(new Player{});
	player_->Init(playerMap);

	followCamera_.reset(new FollowCamera);
	followCamera_->Init();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetCamera(followCamera_->GetCamera());

	skydome_ = std::make_unique<Skydome>();
	skydome_->Init();

	std::unordered_map<std::string, Model *> stageMap{
		{"body", ModelManager::GetInstance()->GetModel("box")}
	};

	for (auto &platform : platform_) {
		platform = std::make_unique<Platform>();
		platform->Init(stageMap);
	}

	platform_[1u]->SetOffset({ 0.f,0.f,20.f });
	platform_[1u]->SetPos(Vector3::zero);
	platform_[1u]->SetRotSpeed({ 0.f,0.25f,0.f });

	platform_[2u]->SetOffset({ 0.f,0.f,40.f });
	platform_[2u]->SetPos(Vector3::zero);
	platform_[2u]->SetRotSpeed({ 0.f,-0.125f,0.f });

	platform_[3u]->SetPos({ 0.f,0.f,60.f });

}

void GameScene::OnExit() {}

void GameScene::Reset() {
	player_->GetWorldTransform().translate = Vector3::zero;
}

void GameScene::Update() {

	const float deltaTime = ImGui::GetIO().DeltaTime;

	if (player_->GetWorldTransform().translate.y < -20.f) {
		Reset();
	}

	ImGui::Begin("Camera");
	followCamera_->ImGuiWidget();
	ImGui::End();

	//camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	//model_->ImGuiWidget();
	transform_.ImGuiWidget();
	ImGui::End();

	for (auto &platform : platform_) {
		platform->Update(deltaTime);
	}

	bool isConnect = false;
	for (auto &platform : platform_) {
		if (Collision::IsHit(player_->GetCollider(), platform->GetCollider())) {
			player_->GetWorldTransform().ConnectParent(platform->GetWorldTransform());
			isConnect = true;
		}
	}
	if (isConnect == false) {
		player_->GetWorldTransform().DisConnectParent();
	}

	//if (Collision::IsHit(player_->GetCollider(), platform_[1]->GetCollider())) {
	//	player_->GetWorldTransform().ConnectParent(platform_[1]->GetWorldTransform());
	//}
	//else {
	//	player_->GetWorldTransform().DisConnectParent();
	//}
	/*
	if (input_->GetDirectInput()->IsTrigger(DIK_P)) {
		player_->GetWorldTransform().ConnectParent(platform_[1]->GetWorldTransform());
	}
	else if (input_->GetDirectInput()->IsTrigger(DIK_O)) {
		player_->GetWorldTransform().DisConnectParent();
	}*/

	TextureManager::GetInstance()->ImGuiWindow();

	light_->ImGuiWidget();

	player_->Update(deltaTime);
	player_->GetWorldTransform().ImGuiWidget();

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
	skydome_->Draw(camera_);
	player_->Draw(camera_);

	for (auto &platform : platform_) {
		platform->Draw(camera_);
	}

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}