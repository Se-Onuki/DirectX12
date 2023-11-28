#include "GameScene.h"

#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include <imgui.h>

#include "TitleScene.h"

#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "../Header/Entity/Component/Rigidbody.h"
#include "../Header/Object/Particle/StarParticle.h"
#include "../Utils/SoLib/SoLib_ImGui.h"

GameScene::GameScene()
{
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();

	collisionDrawer_ = CollisionDrawer::GetInstance();
	particleManager_ = ParticleManager::GetInstance();
}

GameScene::~GameScene()
{
}

void GameScene::OnEnter()
{
	light_.reset(DirectionLight::Create());
	particleManager_->Init(256);
	// model_ = ModelManager::GetInstance()->AddModel("Fence", Model::LoadObjFile("Model/Fence/", "fence.obj"));
	ModelManager::GetInstance()->AddModel("box", Model::LoadObjFile("", "box.obj"))->materialMap_.begin()->second->blendMode_ = Model::BlendMode::kNormal;
	camera_.Init();

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

	Model *const enemyBody =
		ModelManager::GetInstance()->AddModel("enemyBody", Model::LoadObjFile("Model/Boss/", "Boss.obj"));

	for (auto &[key, material] : enemyBody->materialMap_) {
		material->blendMode_ = Model::BlendMode::kNormal;
	}

	std::unordered_map<std::string, Model *> playerMap{
		{"body", playerBody},
		{"head", playerHead},
		{"right", playerRight},
		{"left", playerLeft},
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
		{"body", ModelManager::GetInstance()->GetModel("box")} };

	for (auto &platform : platform_) {
		platform = std::make_unique<Platform>();
		platform->Init(stageMap);
	}

	platform_[1u]->SetOffset({ 0.f, 0.f, 20.f });
	platform_[1u]->SetPos(Vector3::zero);
	platform_[1u]->SetRotSpeed({ 0.f, 0.25f, 0.f });

	platform_[2u]->SetOffset({ 0.f, 0.f, 40.f });
	platform_[2u]->SetPos(Vector3::zero);
	platform_[2u]->SetRotSpeed({ 0.f, -0.125f, 0.f });

	platform_[3u]->SetPos({ 0.f, 0.f, 60.f });

	platform_[4u]->SetScale({ 50.f, 0.1f, 50.f });
	platform_[4u]->SetPos({ 0.f, 0.f, 120.f });

	Model *const goalBox =
		ModelManager::GetInstance()->AddModel("Goal", Model::LoadObjFile("", "box.obj"));
	goalBox->materialMap_["Material"]->materialBuff_->color = { 1.f, 0.f, 0.f, 1.f };

	goal_ = std::make_unique<Entity>();
	auto *const modelComp = goal_->AddComponent<ModelComp>();
	modelComp->AddBone("Body", goalBox, Transform{});
	goal_->transform_.translate = { 0.f, 2.f, 60.f };

	goalCollider_ = std::make_unique<OBB>();
	goalCollider_->size = Vector3::one;

	Reset();

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Init();

	followCamera_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());
}

void GameScene::OnExit() {}

void GameScene::Reset()
{

	enemyList_.clear();

	Model *const enemyBody =
		ModelManager::GetInstance()->GetModel("enemyBody");
	auto enemyModel = std::unordered_map<std::string, Model *>{
		{"body", enemyBody} };

	auto enemy = std::make_unique<Enemy>();
	enemy->Init(enemyModel);
	enemy->GetWorldTransform()->translate.y = 1.f;

	enemy->SetPlatform(platform_[1u].get());

	enemyList_.push_back(std::move(enemy));

	for (uint32_t i = 0u; i < 4u; i++) {
		auto newEnemy = std::make_unique<Enemy>();
		newEnemy->Init(enemyModel);
		newEnemy->GetWorldTransform()->translate = Vector3{ Random::GetRandom<float>(-30.f,30.f), 1.f ,Random::GetRandom<float>(-30.f,30.f) };
		newEnemy->GetWorldTransform()->rotate.y = Random::GetRandom<float>(-Angle::PI, Angle::PI);

		newEnemy->SetPlatform(platform_[4u].get());

		enemyList_.push_back(std::move(newEnemy));
	}

	player_->GetWorldTransform()->DisConnectParent();
	player_->GetWorldTransform()->translate = Vector3::zero;
}

void GameScene::Update()
{

	const float deltaTime = ImGui::GetIO().DeltaTime;

	// オブジェクトを全て走査して、死亡していたら破棄して除外する
	enemyList_.remove_if(
		[](std::unique_ptr<Enemy> &object)
		{
			// もし死んでいたら
			if (not object->GetIsAlive()) {
				// 破棄して除外
				object.reset();
				return true;
			}
			// 死んでいないなら
			// そのまま無視
			return false;
		});


	const Vector3 playerPos = player_->GetWorldTransform()->GetGrobalPos();
	const auto *const weapon = player_->GetWeaponCollider();
	if (weapon) {
		for (auto &enemy : enemyList_) {
			if (Collision::IsHit(enemy->GetCollider(), *weapon)) {
				enemy->HitWeapon(playerPos);
				player_->SetWeaponActive(false);
			}
		}
	}

	for (auto &enemy : enemyList_) {
		if (Collision::IsHit(*player_->GetCollider(), enemy->GetCollider())) {
			Reset();
			break;
		}
	}

	if (player_->GetWorldTransform()->translate.y < -20.f || Collision::IsHit(*goalCollider_.get(), *player_->GetCollider())) {
		Reset();
	}

	ImGui::Begin("Camera");
	followCamera_->ImGuiWidget();
	ImGui::End();

	// camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	// model_->ImGuiWidget();
	ImGui::End();

	for (auto &platform : platform_) {
		platform->Update(deltaTime);
	}

	bool isConnect = false;
	for (auto &platform : platform_) {
		if (Collision::IsHit(*player_->GetCollider(), platform->GetCollider())) {
			player_->GetWorldTransform()->ConnectParent(platform->GetWorldTransform());
			isConnect = true;
		}
	}
	if (isConnect == false) {
		player_->GetWorldTransform()->DisConnectParent();
	}

	TextureManager::GetInstance()->ImGuiWindow();


	lockOn_->Update(enemyList_, camera_);
	player_->Update(deltaTime);

	light_->ImGuiWidget();

	// player_->Update(deltaTime);
	player_->GetWorldTransform()->ImGuiWidget();


	particleManager_->Update(deltaTime);

	followCamera_->Update();
	camera_ = *followCamera_->GetCamera();

	ImGui::Begin("Goal");
	goal_->ImGuiWidget();
	ImGui::End();
	goal_->Update(deltaTime);
	goalCollider_->SetMatrix(goal_->transform_.matWorld_);

	for (auto &enemy : enemyList_) {
		enemy->Update(deltaTime);
	}
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

	Model::SetPipelineType(Model::PipelineType::kModel);

	skydome_->Draw(camera_);
	player_->Draw(camera_);

	for (auto &platform : platform_) {
		platform->Draw(camera_);
	}
	goal_->Draw(camera_);

	for (auto &enemy : enemyList_) {
		enemy->Draw(camera_);
	}

	// ModelManager::GetInstance()->GetModel("box")->Draw(player_->GetWeaponCollisionTransform(), camera_);
	Model::SetPipelineType(Model::PipelineType::kParticle);
	particleManager_->Draw(camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	lockOn_->Draw2D();

	Sprite::EndDraw();

#pragma endregion
}
