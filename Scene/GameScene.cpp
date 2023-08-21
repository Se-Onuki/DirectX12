#include "GameScene.h"
#include "../externals/imgui/imgui.h"
#include "../DirectBase/Base/DirectXCommon.h"
#include "TitleScene.h"
#include "../DirectBase/Input/Input.h"

#include "../Header/Entity/Player.h"
#include "../Header/Model/ModelManager.h"
#include "../Header/Entity/FollowCamera.h"
#include "../Header/Object/Ground.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "../Header/Entity/Component/PlayerComp.h"
#include "../Header/Entity/PlayerBullet.h"

#include "../Header/Entity/Component/Collider.h"


GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	viewProjection_.Init();
	viewProjection_.translation_ = { 0.f,0.f,-15.f };

	Model *const playerBody =
		ModelManager::GetInstance()->AddModel("gunTank", Model::LoadObjFile("Model/gunTank/", "gunTank.obj"));
	ModelManager::GetInstance()->AddModel("sphere", Model::LoadObjFile("", "sphere.obj"));
	ModelManager::GetInstance()->AddModel("ground", Model::LoadObjFile("Model/Ground/", "Ground.obj"));


	Transform bodyTransform = Transform{};
	bodyTransform.translate = { 0.f,0.f,0.f };
	std::unordered_map<std::string, std::pair<Transform, Model *>> playerModel{
		{"body",  {bodyTransform, playerBody}  },
	};

	player_.reset(new Player);
	player_->Init();
	ModelComp *const modelComp = player_->GetComponent<ModelComp>();
	if (modelComp) {
		modelComp->SetModel(playerModel);
	}

	followCamera_.reset(new FollowCamera);
	followCamera_->Init();
	followCamera_->SetTarget(&player_->transform_);

	PlayerComp *const playerComp = player_->GetComponent<PlayerComp>();
	playerComp->SetViewProjection(followCamera_->GetViewProjection());
	playerComp->SetGameScene(this);

	ground_.reset(new Ground);
	ground_->Init();

	light_.reset(DirectionLight::Create());
}

void GameScene::OnExit() {
}

void GameScene::Update() {
	//const DirectInput *const directInput = DirectInput::GetInstance();

	pBulletList_.remove_if([](std::unique_ptr<PlayerBullet> &bullet) {
		if (!bullet->GetActive()) {
			bullet.reset();
			return true;
		}
		return false;
		}
	);

#pragma region AddCollisionManager
	collisionManager_->clear();

	collisionManager_->push_back(player_.get());
	for (auto &pBullet : pBulletList_) {
		collisionManager_->push_back(pBullet.get());
	}
	/*for (auto &enemy : enemyList_) {
		collisionManager_->push_back(enemy.get());
	}

	for (auto &eBullet : enemyBulletList_) {
		collisionManager_->push_back(eBullet.get());
	}*/

	collisionManager_->ChackAllCollision();

#pragma endregion


	player_->Update();
	followCamera_->Update();

	for (auto &pBullet : pBulletList_) {
		pBullet->Update();
	}

	viewProjection_.matView_ = followCamera_->GetViewMatrix();
	viewProjection_.matProjection_ = followCamera_->GetProjectionMatrix();

	followCamera_->ImGuiWidget();

	viewProjection_.TransferMatrix();
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
	ground_->Draw(viewProjection_);
	player_->Draw(viewProjection_);

	for (auto &pBullet : pBulletList_) {
		pBullet->Draw(viewProjection_);
	}

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}

void GameScene::AddPlayerBullet(PlayerBullet *newBullet) {
	//std::unique_ptr<PlayerBullet> bulletPtr{ newBullet };
	pBulletList_.emplace_back(newBullet);
	/*auto &uPtr = pBulletList_.back();
	uPtr.reset(newBullet);*/
}
