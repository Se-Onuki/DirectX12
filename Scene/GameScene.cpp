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

#include "../Header/Object/Fade.h"

#include "TitleScene.h"
#include "StageSelectScene.h"

#include "../Header/Object/Pose/PoseManager.h"

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

	static auto *const modelManager = ModelManager::GetInstance();
	static auto *const particleManager = ParticleManager::GetInstance();
	static auto *const particleEmitterManager = ParticleEmitterManager::GetInstance();

	// カメラマネージャーの初期化
	cameraManager_->Init();
	// テスト用新規カメラを追加
	cameraManager_->AddCamera("TestCamera");

	// パーティクルマネージャの初期化
	particleManager->Init(256); // パーティクルの最大数は256
	// パーティクルエミッタマネージャーの初期化
	particleEmitterManager->Init();

	//model_ = ModelManager::GetInstance()->GetModel("Plane");

	//BaseTransform transform;
	//transform_ = transform;

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

	// ポーズ画面マネージャー初期化
	PoseManager::GetInstance()->Init();

#pragma region Player

	/*Model *const boxModel = */ModelManager::GetInstance()->GetModel("Box");
	player_ = std::make_unique<Entity>();
	//auto*const rigidbody =
	player_->AddComponent<Rigidbody>();
	//auto *const modelComp =
	player_->AddComponent<ModelComp>();
	//modelComp->AddBone("Body", boxModel);

	player_->AddComponent<PlayerComp>();
	player_->AddComponent<PlayerAnimComp>();

	ParticleEmitter *emitter = nullptr;
	emitter = particleEmitterManager->CreateEmitter<StarParticle>("PlayerLing");
	emitter->targetTransform_ = &player_->transform_;
	emitter->offset_ = { 0.0f, 0.5f, 0.0f };

#pragma endregion

#pragma region FollowCamera

	followCamera_ = std::make_unique<Entity>();
	auto *const followComp = followCamera_->AddComponent<FollowCameraComp>();
	followComp->SetTarget(&player_->transform_);

#pragma endregion

	player_->GetComponent<PlayerComp>()->SetFollowCamera(followComp);

	/*cameraList_[0u] = &followComp->GetCamera();
	cameraList_[1u] = &camera_;*/

	// フェードイン開始
	Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 0.0f }, 1.0f);

}

void GameScene::OnExit() {}

void GameScene::Update() {

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	static auto *const colliderManager = CollisionManager::GetInstance();
	static auto *const particleManager = ParticleManager::GetInstance();
	static auto *const particleEmitterManager = ParticleEmitterManager::GetInstance();
	static const auto *const keyBoard = input_->GetDirectInput();

	colliderManager->clear();

	TextureManager::GetInstance()->ImGuiWindow();

	particleEmitterManager->Update(deltaTime);
	particleManager->Update(deltaTime);

	levelManager->Update(deltaTime);
	followCamera_->AddComponent<FollowCameraComp>()->SetLine(levelManager->GetStageLine());

	ImGui::Begin("LevelManager");
	levelManager->ImGuiWidget();

	ImGui::End();

	if (Input::GetInstance()->GetDirectInput()->IsPress(DIK_0)) {
		sceneManager_->ChangeScene(std::unique_ptr<GameScene>(), 1.f);
	}

	// ポーズ画面マネージャー初期化
	PoseManager::GetInstance()->Update(deltaTime);
	// Startボタンをおしたらメニューを展開
	if (Input::GetInstance()->GetXInput()->IsPress(KeyCode::START)) {
		PoseManager::GetInstance()->DeployPoseMenu();
	}

	player_->Update(deltaTime);

	ImGui::Begin("Player");
	if (ImGui::Button("Reset")) {
		player_->Reset();
	}
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

	//camera_.translation_ = player_->transform_.translate + Vector3{ 0.f,1.f,-15.f };

	/*if (keyBoard->IsTrigger(DIK_0)) {
		if (++cameraTarget_ == cameraList_.end()) {
			cameraTarget_ = cameraList_.begin();
		}
	}*/

	// カメラマネージャーの更新
	cameraManager_->Update(deltaTime);

#ifdef _DEBUG // デバッグ時のみImGuiを描画
	// カメラマネージャーのImGuiを表示
	cameraManager_->DisplayImGui();

	// スペースを押すと次のシーンへ
	if (keyBoard->IsTrigger(DIK_RSHIFT)) {
		// フェードアウト開始
		Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
		// 指定した秒数後シーンチェンジ
		sceneManager_->ChangeScene(std::make_unique<StageSelectScene>(), 1.0f);
	}

	// スペースを押すと次のシーンへ
	if (keyBoard->IsTrigger(DIK_M)) {
		// フェードアウト開始
		Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
		// 指定した秒数後シーンチェンジ
		sceneManager_->ChangeScene(std::make_unique<TitleScene>(), 1.0f);
	}
#endif // _DEBUG // デバッグ時のみImGuiを描画


	//playerAnim_->Update(deltaTime);

	light_->ImGuiWidget();

	//transform_->ImGuiWidget();
	//transform_->UpdateMatrix();

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

	const auto &camera = *cameraManager_->GetUseCamera();

	//model_->Draw(transform_, camera_);
	levelManager->Draw(camera);

	player_->Draw(camera);
	// 描画
	//playerAnim_->Draw(camera_);

	Model::SetPipelineType(Model::PipelineType::kParticle);
	static auto *const particleManager = ParticleManager::GetInstance();

	// 複数モデルのパーティクルを、それぞれの集合ごとに描画
	particleManager->Draw(camera);

	// モデルの描画


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	/*sprite_->Draw();*/

	// ポーズ画面マネージャー初期化
	PoseManager::GetInstance()->Draw();

	// フェード演出描画
	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}