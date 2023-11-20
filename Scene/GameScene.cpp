#include "GameScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"

#include "TitleScene.h"

#include "../Header/Entity/Component/ModelComp.h"
#include "../Utils/SoLib/SoLib_ImGui.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Header/Entity/Component/Rigidbody.h"
#include "../Header/Entity/Component/PlayerComp.h"
#include "../Header/Entity/Component/Collider.h"
#include "../Header/Entity/Component/PlayerAnimComp.h"
#include "../Header/Entity/Component/FollowCameraComp.h"

#include "../Header/Object/Particle/ParticleEmitterManager.h"

#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	light_.reset(DirectionLight::Create());

	static auto* const modelManager = ModelManager::GetInstance();
	static auto* const particleManager = ParticleManager::GetInstance();
	static auto* const particleEmitterManager = ParticleEmitterManager::GetInstance();

	// カメラマネージャーの初期化
	cameraManager_->Init();

	// パーティクルマネージャの初期化
	particleManager->Init(256); // パーティクルの最大数は256
	// パーティクルエミッタマネージャーの初期化
	particleEmitterManager->Init(); 

	// モデルの読み込み
	modelManager->CreateDefaultModel(); // デフォルトモデルの読み込み
	modelManager->AddModel("Box", Model::LoadObjFile("", "box.obj"));
	modelManager->AddModel("RedBox", Model::LoadObjFile("", "box.obj"))->materialMap_["Material"]->materialBuff_->color = Vector4{ 1.f,0.f,0.f,1.f };
	modelManager->AddModel("GrassModel", Model::LoadObjFile("", "box.obj"))->materialMap_["Material"]->materialBuff_->color = Vector4{ 0.f,0.5f,0.f,1.f };
	modelManager->AddModel("DirtModel", Model::LoadObjFile("", "box.obj"))->materialMap_["Material"]->materialBuff_->color = Vector4{ 0.5f,0.5f,0.f,1.f };

	modelManager->AddModel("PlayerBody", Model::LoadObjFile("Model/PlayerModel/Body/", "Body.obj")); // プレイヤーの体
	modelManager->AddModel("PlayerEye", Model::LoadObjFile("Model/PlayerModel/Eye/", "Eye.obj")); // プレイヤーの瞳
	modelManager->AddModel("PlayerHelmet", Model::LoadObjFile("Model/PlayerModel/Helmet/", "Helmet.obj")); // プレイヤーのヘルメット
	modelManager->AddModel("PlayerLing", Model::LoadObjFile("Model/PlayerModel/Ling/", "Ling.obj")); // プレイヤーの輪っか
	modelManager->AddModel("PlayerArm_L", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_L.obj")); // プレイヤーの左腕
	modelManager->AddModel("PlayerArm_R", Model::LoadObjFile("Model/PlayerModel/CharaArm/", "Arm_R.obj")); // プレイヤーの右腕
	modelManager->AddModel("PlayerFoot_L", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_L.obj")); // プレイヤーの左足
	modelManager->AddModel("PlayerFoot_R", Model::LoadObjFile("Model/PlayerModel/Foot/", "Foot_R.obj")); // プレイヤーの右足

	//model_ = ModelManager::GetInstance()->GetModel("Plane");

	//BaseTransform transform;
	//transform_ = transform;
	camera_.Init();

	//particleManager->AddParticle(modelManager->GetModel("Plane"), std::make_unique<TestParticle>());

	/*sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });*/

	levelManager = LevelElementManager::GetInstance();

	levelManager->Init();
	levelManager->AddBlock(1u, AABB{ .min{-10.f,-1.f,-10.f}, .max{10.f,1.f,10.f} }.AddPos({ 0.f,-3.f,20.f }));

	levelManager->AddBlock(0u, AABB{ .min{-10.f,-1.f,-10.f}, .max{10.f,1.f,10.f} }.AddPos({ 0.f,-3.f,0.f }));
	levelManager->AddBlock(0u, AABB{ .min{-1.f,-3.f,-1.f}, .max{1.f,3.f,1.f} }.AddPos({ 0.f,5.f,0.f }));
	//levelManager->blockCollider_[0u].AddRotate(180._deg);
	// levelManager->blockCollider_[0u].center_.translate.z = 180._deg;

	levelManager->CalcCollision();

#pragma region Player

	Model* const boxModel = ModelManager::GetInstance()->GetModel("Box");
	player_ = std::make_unique<Entity>();
	//auto*const rigidbody =
	player_->AddComponent<Rigidbody>();
	auto* const modelComp =
		player_->AddComponent<ModelComp>();
	modelComp->AddBone("Body", boxModel);

	player_->AddComponent<PlayerComp>();

	ParticleEmitter* emitter = nullptr;
	emitter = particleEmitterManager->CreateEmitter<StarParticle>("PlayerLing");
	emitter->targetTransform_ = &player_->transform_;

#pragma endregion

#pragma region FollowCamera

	followCamera_ = std::make_unique<Entity>();
	auto* const followComp = followCamera_->AddComponent<FollowCameraComp>();
	followComp->SetTarget(&player_->transform_);

#pragma endregion

	player_->GetComponent<PlayerComp>()->SetFollowCamera(followComp);

	cameraList_[0u] = &followComp->GetCamera();
	cameraList_[1u] = &camera_;
}

void GameScene::OnExit() {}

void GameScene::Update() {

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	static auto* const colliderManager = CollisionManager::GetInstance();
	static auto* const particleManager = ParticleManager::GetInstance();
	static auto* const particleEmitterManager = ParticleEmitterManager::GetInstance();
	static const auto* const keyBoard = input_->GetDirectInput();

	colliderManager->clear();

	TextureManager::GetInstance()->ImGuiWindow();

	particleEmitterManager->Update(deltaTime);
	particleManager->Update(deltaTime);

	levelManager->Update(deltaTime);
	followCamera_->AddComponent<FollowCameraComp>()->SetLine(levelManager->GetStageLine());

	ImGui::Begin("LevelManager");
	levelManager->ImGuiWidget();

	ImGui::End();


	player_->Update(deltaTime);

	ImGui::Begin("Player");
	player_->ImGuiWidget();
	ImGui::End();

	Vector3 euler{};
	if (keyBoard->IsPress(DIK_RIGHT)) {
		euler += Vector3::up * -5._deg;
	}
	if (keyBoard->IsPress(DIK_LEFT)) {
		euler += Vector3::up * 5._deg;
	}

	followCamera_->GetComponent<FollowCameraComp>()->AddRotate(euler);
	followCamera_->ImGuiWidget();
	followCamera_->Update(deltaTime);

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();

	//camera_.translation_ = player_->transform_.translate + Vector3{ 0.f,1.f,-15.f };
	camera_.UpdateMatrix();

	if (keyBoard->IsTrigger(DIK_0)) {
		if (++cameraTarget_ == cameraList_.end()) {
			cameraTarget_ = cameraList_.begin();
		}
	}

	// カメラマネージャーの更新
	cameraManager_->Update();

#ifdef _DEBUG // デバッグ時のみImGuiを描画
	// カメラマネージャーのImGuiを表示
	cameraManager_->DisplayImGui();
#endif // _DEBUG // デバッグ時のみImGuiを描画


	//playerAnim_->Update(deltaTime);

	light_->ImGuiWidget();

	//transform_->ImGuiWidget();
	//transform_->UpdateMatrix();

}

void GameScene::Draw()
{
	DirectXCommon* const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* const commandList = dxCommon->GetCommandList();

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

	const auto& camera = **cameraTarget_;

	//model_->Draw(transform_, camera_);
	levelManager->Draw(camera);

	player_->Draw(camera);
	// 描画
	//playerAnim_->Draw(camera_);

	Model::SetPipelineType(Model::PipelineType::kParticle);
	static auto* const particleManager = ParticleManager::GetInstance();

	// 複数モデルのパーティクルを、それぞれの集合ごとに描画
	particleManager->Draw(camera);

	// モデルの描画


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	/*sprite_->Draw();*/

	Sprite::EndDraw();

#pragma endregion

}