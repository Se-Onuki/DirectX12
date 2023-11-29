#include "GameScene.h"

#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include <imgui.h>

#include "TitleScene.h"

#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Header/Entity/Component/Collider.h"
#include "../Header/Entity/Component/FollowCameraComp.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "../Header/Entity/Component/PlayerAnimComp.h"
#include "../Header/Entity/Component/PlayerComp.h"
#include "../Header/Entity/Component/Rigidbody.h"
#include "../Utils/SoLib/SoLib_ImGui.h"

#include "../Header/Object/Particle/ParticleEmitterManager.h"
#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/TutorialManager.h"

#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../StarItemComp.h"

#include "../Header/Object/Fade.h"

#include "TitleScene.h"
#include "StageSelectScene.h"

#include "../Header/Object/Pose/PoseManager.h"
#include "../Header/Object/SkyDome/SkyDome.h"

#include "../Header/Entity/Component/GoalAnimations/GoalAnimComp.h"
#include "../Header/Entity/Component/PlayerState/RotatingState.h"

// 静的なメンバ変数の実体を宣言
uint32_t GameScene::gameSceneBGM_ = 0u;

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

	static auto *const blockManager = BlockManager::GetInstance();

	blockManager->Init(8192u);

	// カメラマネージャーの初期化
	cameraManager_->Init();
	// テスト用新規カメラを追加
	cameraManager_->AddCamera("TestCamera");

	// パーティクルマネージャの初期化
	particleManager->Init(512u); // パーティクルの最大数は256
	// パーティクルエミッタマネージャーの初期化
	particleEmitterManager->Init();

	//model_ = ModelManager::GetInstance()->GetModel("Plane");

	//BaseTransform transform;
	//transform_ = transform;

	//particleManager->AddParticle(modelManager->GetModel("Plane"), std::make_unique<TestParticle>());

	/*sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });*/

	if (gameSceneBGM_ == 0u) {
		gameSceneBGM_ = audio_->LoadWave("resources/Audio/BGM/ingameBGM.wav");
	}

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

#pragma endregion

#pragma region LevelManager

	levelManager = LevelElementManager::GetInstance();

	levelManager->Init();

	levelManager->SetPlayer(player_.get());

	levelManager->LoadData(StageSelectManager::GetInstance()->GetSelectedStageNumber());

	levelManager->CalcCollision();

#pragma endregion

#pragma region FollowCamera

	followCamera_ = std::make_unique<Entity>();
	auto *const followComp = followCamera_->AddComponent<FollowCameraComp>();
	followComp->SetTarget(&player_->transform_);

	cameraManager_->SetUseCamera(followComp->GetCamera());

#pragma endregion

	player_->GetComponent<PlayerComp>()->SetFollowCamera(followComp);

	/*starItem_ = std::make_unique<Entity>();

	starItem_->AddComponent<StarItemComp>();*/

	/*cameraList_[0u] = &followComp->GetCamera();
	cameraList_[1u] = &camera_;*/

	sceneChanging_ = false;

	skyDome_ = std::make_unique<Entity>();
	skyDome_->AddComponent<SkyDome>();

	// インゲームUI
	inGameUI_ = std::make_unique<InGameUIManager>();
	// 初期化
	inGameUI_->Init(levelManager->GetMaxStarCount());
	// プレイヤーコンポーネントのセットIn
	inGameUI_->SetPlayerComp(*player_->GetComponent<PlayerComp>());
	// フェードイン開始
	Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 0.0f }, 1.0f);

	levelManager->AddUndoLog(nullptr);

	TutorialManager::GetInstance()->Init();

	player_->GetComponent<PlayerComp>()->SetInGameUIManager(inGameUI_.get());
	levelManager->pInGameUI_ = inGameUI_.get();

}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	static auto *const colliderManager = CollisionManager::GetInstance();
	static auto *const particleManager = ParticleManager::GetInstance();
	static auto *const particleEmitterManager = ParticleEmitterManager::GetInstance();
	static const auto *const keyBoard = input_->GetDirectInput();
	static const auto *const gamePad = input_->GetXInput();

	// ポーズ画面マネージャー初期化
	PoseManager::GetInstance()->Update(deltaTime);

	// BGM再生
	if (audio_->IsPlaying(voiceGameSceneBGMHandle_) == 0 || voiceGameSceneBGMHandle_ == -1) {
		voiceGameSceneBGMHandle_ = audio_->PlayWave(gameSceneBGM_, false, BGMVolume_);
	}

	const auto &goalList = levelManager->GetGoalList();

	bool isGoaled = false;

	for (Entity *const goal : goalList) {
		isGoaled = goal->GetComponent<GoalAnimComp>()->GetIsPlay();
		if (isGoaled) {
			break;
		}
	}

	// Startボタンをおしたらメニューを展開
	if ((Input::GetInstance()->GetDirectInput()->IsTrigger(DIK_ESCAPE) || Input::GetInstance()->GetXInput()->IsTrigger(KeyCode::START)) && not PoseManager::GetInstance()->GetIsActive() && not isGoaled && Fade::GetInstance()->GetSprite()->GetColor().w <= 0.f) {
		if (PoseManager::GetInstance()->GetPoseState() == PoseManager::kNone) {
			PoseManager::GetInstance()->DeplayPoseMenu();
		}

	}
	if (not PoseManager::GetInstance()->GetIsActive()) {
		colliderManager->clear();

		TextureManager::GetInstance()->ImGuiWindow();

		particleEmitterManager->Update(deltaTime);

		player_->Update(deltaTime);

		levelManager->Update(deltaTime);
		followCamera_->AddComponent<FollowCameraComp>()->SetLine(levelManager->GetStageLine());

		ImGui::Begin("LevelManager");
		levelManager->ImGuiWidget();

		ImGui::End();

		for (Entity *const goal : levelManager->GetGoalList()) {
			if (goal->GetComponent<GoalAnimComp>()->GetIsPlay()) {
				player_->GetComponent<PlayerComp>()->SetIsGoaled(true);
				break;
			}
		}

		skyDome_->Update(deltaTime);

		//goal_->Update(deltaTime);

		ImGui::Begin("Player");
		ImGui::Text("%d", TutorialManager::GetInstance()->GetProgress());
		if (ImGui::Button("Reset")) {
			player_->Reset();
		}
		player_->ImGuiWidget();
		ImGui::End();

		Vector3 euler{};
		if (std::abs(gamePad->GetState()->stickR_.x) > 0.1f) {
			euler += Vector3::up * 3._deg * gamePad->GetState()->stickR_.x;
		}
		if (std::abs(gamePad->GetState()->stickR_.y) > 0.1f) {
			euler -= Vector3::right * 3._deg * gamePad->GetState()->stickR_.y;
		}
		if (euler.LengthSQ() <= 0.f) {
			if (keyBoard->IsPress(DIK_RIGHT)) {
				euler += Vector3::up * -3._deg;
			}
			if (keyBoard->IsPress(DIK_LEFT)) {
				euler += Vector3::up * +3._deg;
			}
			if (keyBoard->IsPress(DIK_UP)) {
				euler += Vector3::right * +3._deg;
			}
			if (keyBoard->IsPress(DIK_DOWN)) {
				euler += Vector3::right * -3._deg;
			}
		}

		followCamera_->GetComponent<FollowCameraComp>()->AddRotate(euler);
		followCamera_->ImGuiWidget();
		followCamera_->Update(deltaTime);

		auto *const rotateState = dynamic_cast<PlayerRotatingState *>(player_->GetComponent<PlayerComp>()->GetState().get());
		if (rotateState) {
			rotateState->AddRotate(euler);
		}

		static auto *const blockManager = BlockManager::GetInstance();

		// 複数モデルのパーティクルを、それぞれの集合ごとに描画
		blockManager->Update();

		inGameUI_->Update(deltaTime);

		// カメラマネージャーの更新
		cameraManager_->Update(deltaTime);

		particleManager->Update(deltaTime);

	}
	if (PoseManager::GetInstance()->GetPoseState() == PoseManager::kReturnCheckPoint) {
		if (not Fade::GetInstance()->GetTimer()->IsActive()) {
			Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 0.25f);
		}
		if (levelManager->Undo()) {
			cameraManager_->SetUseCamera("FollowCamera");
			PoseManager::GetInstance()->SetPoseState(PoseManager::kNone);
		}
	}
	if (PoseManager::GetInstance()->GetPoseState() == PoseManager::kRetry) {
		if (!sceneChanging_) {
			Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
			sceneManager_->ChangeScene(std::make_unique<GameScene>(), 1.f);
			sceneChanging_ = true;
		}
	}
	// ステージ選択に戻るよう指示されていたら戻る
	if (PoseManager::GetInstance()->GetPoseState() == PoseManager::kReturnStageSelect) {
		if (!sceneChanging_) {
			// フェードアウト開始
			Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
			// 指定した秒数後シーンチェンジ
			sceneManager_->ChangeScene(std::make_unique<StageSelectScene>(), 1.0f);
			// シーン遷移中
			sceneChanging_ = true;
		}
	}


	// ゴール演出が終了している場合
	if (not sceneChanging_) {
		for (const auto goal : levelManager->GetGoalList()) {
			if (goal->GetComponent<GoalAnimComp>()->GetIsEnd()) {

				// フェードアウト開始
				Fade::GetInstance()->Start({ 0.0f, 0.0f }, { 0.0f,0.0f, 0.0f, 1.0f }, 1.0f);
				// 指定した秒数後シーンチェンジ
				sceneManager_->ChangeScene(std::make_unique<StageSelectScene>(), 1.0f);
				// シーン遷移中
				sceneChanging_ = true;

				break;
			}
		}
	}

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
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();


	const auto &camera = *cameraManager_->GetUseCamera();

#pragma region モデル描画

	Model::StartDraw(commandList);

	light_->SetLight(commandList);

	Model::SetPipelineType(Model::PipelineType::kModel);

	//model_->Draw(transform_, camera_);

	//player_->Draw(camera);

	//starItem_->Draw(camera);
	// 描画
	//playerAnim_->Draw(camera_);

	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);
	static auto *const blockManager = BlockManager::GetInstance();

	// 複数モデルのパーティクルを、それぞれの集合ごとに描画
	blockManager->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kModel);

	light_->SetLight(commandList);

	levelManager->Draw(camera);

	player_->Draw(camera);

	//goal_->Draw(camera);

	skyDome_->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kParticle);
	static auto *const particleManager = ParticleManager::GetInstance();

	// 複数モデルのパーティクルを、それぞれの集合ごとに描画
	particleManager->Draw(camera);


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	/*sprite_->Draw();*/

	inGameUI_->Draw();

	// ポーズ画面マネージャー初期化
	PoseManager::GetInstance()->Draw();

	// フェード演出描画
	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}