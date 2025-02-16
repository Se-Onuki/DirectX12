/// @file GameScene.cpp
/// @brief ゲームの処理を実装する
/// @author ONUKI seiya
#include "GameScene.h"

#include "ResultScene.h"

#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../Header/Object/Fade.h"
#include "../../Engine/Utils/SoLib/SoLib.h"
#include <imgui.h>
#include "TitleScene.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "../../Engine/Utils/Math/Angle.h"
#include "../Engine/ECS/System/Systems.h"
#include "../Engine/LevelEditor/LevelData.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../Engine/ECS/System/FunctionalSystem.h"
#include "../Engine/ECS/System/NewSystems.h"
#include "../Engine/DirectBase/Model/SkeletonAnimation/Skeleton.h"
#include "../Engine/ResourceManager/ResourceLoader.h"

GameScene::GameScene() {
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
	blockRender_ = BlockManager::GetInstance();
	skinModelRender_ = SkinModelListManager::GetInstance();
	modelHandleRender_ = ModelHandleListManager::GetInstance();
	skinModelHandleRender_ = SkinModelHandleListManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {

	pDxCommon_ = DirectXCommon::GetInstance();
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader, ShaderSource>::GetInstance();
	texStrage_ = SolEngine::FullScreenTextureStrage::GetInstance();

	compRegistry_ = ECS::ComponentRegistry::GetInstance();

	numberRender_ = SolEngine::NumberRender::Generate(TextureManager::Load("UI/Number.png"));

	SolEngine::ResourceObjectManager<SolEngine::AssimpData> *const assimpManager = SolEngine::ResourceObjectManager<SolEngine::AssimpData>::GetInstance();
	SolEngine::ResourceObjectManager<SolEngine::ModelData> *const modelDataManager = SolEngine::ResourceObjectManager<SolEngine::ModelData>::GetInstance();
	SolEngine::ResourceObjectManager<SolEngine::Skeleton> *const skeletonManager = SolEngine::ResourceObjectManager<SolEngine::Skeleton>::GetInstance();

	auto playerAssimp = assimpManager->Load({ "Model/human/", "sneakWalk.gltf" });
	auto playerModel = modelDataManager->Load({ playerAssimp });
	auto playerSkeleton = skeletonManager->Load({ playerAssimp });

	auto ghostAssimp = assimpManager->Load({ "Model/Cute Animated Monsters Pack/", "Ghost.gltf" });
	auto ghostModel = modelDataManager->Load({ ghostAssimp });

	light_ = DirectionLight::Generate();

	blockRender_->Init(1024u);
	skinModelRender_->Init(1024u);
	modelHandleRender_->Init(1024u);
	skinModelHandleRender_->Init(1024u);

	SolEngine::Resource::ResourceLoadManager resourceLoadManager;
	SoLib::IO::File file{ "resources/Scene/GameScene.jsonc" };
	nlohmann::json sceneJson;
	file >> sceneJson;
	resourceLoadManager.Init(sceneJson["Resources"]);
	resourceLoadManager.Load();

	spawner_.clear();

	auto animationManager = SolEngine::ResourceObjectManager<SolEngine::Animation>::GetInstance();

	animation_ = animationManager->Load(assimpManager->Load({ "Model/human/", "walk.gltf" }));
	attackAnimation_ = animationManager->Load({ assimpManager->Load({ "Model/human/", "Attack.gltf" }), 1 });

	skinModel_ = SkinModel::MakeSkinModel(*playerModel, *playerSkeleton);

	attackModel_ = ModelManager::GetInstance()->AddModel("AttackModel", Model::CreatePlane());
	{
		auto *const mtr = attackModel_->materialMap_.begin()->second.get();
		mtr->texHandle_ = TextureManager::Load("UI/fullCircle.png");
		mtr->blendMode_ = Model::BlendMode::kAdd;
	}

	// カーソルのモデル
	auto cursor = ModelManager::GetInstance()->AddModel("Cursor", Model::CreatePlane());
	{
		auto *const mtr = cursor->materialMap_.begin()->second.get();
		mtr->texHandle_ = TextureManager::Load("UI/circle.png");
		mtr->blendMode_ = Model::BlendMode::kAdd;
	}
	// カーソルのモデル
	auto inCursor = ModelManager::GetInstance()->AddModel("InCursor", Model::CreatePlane());
	{
		auto *const mtr = inCursor->materialMap_.begin()->second.get();
		mtr->texHandle_ = TextureManager::Load("UI/fullCircle.png");
		mtr->blendMode_ = Model::BlendMode::kAdd;
	}

	cameraManager_->Init();
	followCamera_ = cameraManager_->AddCamera("FollowCamera");
	cameraManager_->SetUseCamera(followCamera_);
	cameraManager_->Update(0.f);

	playerPrefab_ = std::make_unique<ECS::Prefab>();

	*playerPrefab_ += ECS::IsAlive{};
	*playerPrefab_ += ECS::ScaleComp{ .scale_ = Vector3::one * 2.5f };
	*playerPrefab_ += ECS::QuaternionRotComp{};
	*playerPrefab_ += ECS::PositionComp{ .position_ {.x = 0.1f} };
	*playerPrefab_ += ECS::InputFlagComp{};
	*playerPrefab_ += ECS::TransformMatComp{};
	*playerPrefab_ += ECS::ModelAnimator{ .animateList_{{ animation_, animation_, attackAnimation_, attackAnimation_}},.animatior_ = animation_ };
	*playerPrefab_ += ECS::SkinModel{ .skinModel_ = skinModel_.get() };
	*playerPrefab_ += ECS::ModelComp{ .model_ = {playerModel} };
	*playerPrefab_ += ECS::VelocityComp{};
	*playerPrefab_ += ECS::AccelerationComp{};
	*playerPrefab_ += ECS::GravityComp{};
	*playerPrefab_ += ECS::SphereCollisionComp{ .collision_ = Sphere{.centor = Vector3::up, .radius = 1.f } };
	*playerPrefab_ += ECS::PlayerTag{};
	*playerPrefab_ += ECS::IsLanding{};
	*playerPrefab_ += ECS::AttackCollisionComp{};
	*playerPrefab_ += ECS::EntityState{};
	*playerPrefab_ += ECS::HealthComp::Create(120);
	*playerPrefab_ += ECS::InvincibleTime{ .timer_{ 1.f, false } };
	*playerPrefab_ += ECS::AirResistance{ .resistance = (3.6f / 60.f) };
	*playerPrefab_ += ECS::CursorComp{ .model_ = cursor, .inModel_ = inCursor };
	*playerPrefab_ += ECS::AttackStatus{ .radius_ = 10.f };
	*playerPrefab_ += ECS::AttackPower{ .power_ = 40 };
	*playerPrefab_ += ECS::AttackCooltime{ .cooltime_ = { 1.0f, false } };
	*playerPrefab_ += ECS::Experience{};
	*playerPrefab_ += ECS::HasShadow{};
	*playerPrefab_ += ECS::ArrowShooter{ .count_ = 0, .needTime_ = 1.f };

	newWorld_.CreateEntity(*playerPrefab_);

	enemyPrefab_ = std::make_unique<ECS::Prefab>();
	*enemyPrefab_ += ECS::IsAlive{};
	*enemyPrefab_ += ECS::ScaleComp{ };
	*enemyPrefab_ += ECS::QuaternionRotComp{};
	*enemyPrefab_ += ECS::PositionComp{ .position_{0.f, 1.f, 10.f} };
	*enemyPrefab_ += ECS::TransformMatComp{};
	*enemyPrefab_ += ECS::ModelComp{ .model_ = {ghostModel} };
	*enemyPrefab_ += ECS::GravityComp{};
	*enemyPrefab_ += ECS::SphereCollisionComp{ .collision_ = Sphere{.radius = 1.f } };
	*enemyPrefab_ += ECS::EnemyTag{};
	*enemyPrefab_ += ECS::HealthComp::Create(100);
	*enemyPrefab_ += ECS::HealthBarComp{};
	*enemyPrefab_ += ECS::AttackPower{ .power_ = 10 };
	*enemyPrefab_ += ECS::AttackCooltime{ .cooltime_ = { 5.f, false } };
	*enemyPrefab_ += ECS::GhostModel{};
	*enemyPrefab_ += ECS::UnRender{};
	*enemyPrefab_ += ECS::HasShadow{};
	*enemyPrefab_ += ECS::InvincibleTime{};
	*enemyPrefab_ += ECS::Rigidbody{};
	*enemyPrefab_ += ECS::DamageCounter{};

	//entityManager_->CreateEntity(*enemyPrefab_);
	newWorld_.CreateEntity(*enemyPrefab_);

	// カメラのオフセット位置
	const Vector3 cameraOffset{ 0.f,1.f,-1.f };

	ECS::Prefab followCamera;
	followCamera += ECS::FollowCamera{ .rotation_ = Quaternion::LookAt(-cameraOffset), .offset_ {.z = -40.f} };
	followCamera += ECS::PositionComp{};

	//entityManager_->CreateEntity(followCamera);
	newWorld_.CreateEntity(followCamera);

	attackSound_ = audio_->LoadMP3("resources/Audio/SE/Player/startRotate1.mp3");

	ground_.Init();

	spawnTimer_.Start();
	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);

	healthBar_ = std::make_unique<HealthBar>();
	healthBar_->Init();

	expBar_ = std::make_unique<HealthBar>();
	expBar_->Init();

	expBar_->SetColor(0xFFFF00FF, 0xFFFFFFFF);

	expBar_->SetCentor(Vector2{ static_cast<float>(WinApp::kWindowWidth) * vExpUICentorMul_->x, static_cast<float>(WinApp::kWindowHeight) * vExpUICentorMul_->y } + *vExpUICentorDiff_);
	expBar_->SetScale(Vector2{ static_cast<float>(WinApp::kWindowWidth) * vExpUIScaleMul_->x + vExpUIScaleDiff_->x, static_cast<float>(WinApp::kWindowHeight) * vExpUIScaleMul_->y + vExpUIScaleDiff_->y });

	ghostRenderer_.Init();
	ghostRenderer_.SetModelData(ghostModel);

	auto shadowAssimp = assimpManager->Load({ "","plane.obj" });
	auto shadowModel = modelDataManager->Load({ shadowAssimp });
	auto &shadowMesh = shadowModel->meshHandleList_.front();
	shadowMesh->materialhandle_->blendMode_ = Model::BlendMode::kNormal;
	static_cast<Matrix4x4 &>(shadowModel->rootNode_) = Matrix4x4::Affine(Vector3::one * 1.5f, { -SoLib::Angle::Rad90, 0.f, 0.f }, Vector3{ 0.f,0.1f,0.f });
	shadowRenderer_.Init();
	shadowRenderer_.SetModelData(shadowModel);

	auto orbAssimp = assimpManager->Load({ "Model/", "Orb.obj" });
	auto orbModel = modelDataManager->Load({ orbAssimp });
	auto &orbMesh = orbModel->meshHandleList_.front();
	orbMesh->materialhandle_->blendMode_ = Model::BlendMode::kNormal;

	expRender_.Init();
	expRender_.SetModelData(orbModel);

	auto attackAssimp = assimpManager->Load({ "","Attack.obj" });
	auto attackModel = modelDataManager->Load({ attackAssimp });
	auto &attackMesh = attackModel->meshHandleList_.front();
	attackMesh->materialhandle_->blendMode_ = Model::BlendMode::kNormal;
	static_cast<Matrix4x4 &>(attackModel->rootNode_) = Matrix4x4::Affine(Vector3::one * 2.f, { -SoLib::Angle::Rad90, 0.f, 0.f }, Vector3{ 0.f,0.1f,0.f });

	auto arrowAssimp = assimpManager->Load({ "Model/Shuriken/","shuriken.glb" });
	auto arrowModel = modelDataManager->Load({ arrowAssimp });

	// 攻撃の描画
	attackRender_.Init();
	attackRender_.SetModelData(attackModel);

	// 飛び道具の描画
	arrowAttackRender_.Init();
	arrowAttackRender_.SetModelData(arrowModel);

	for (auto &bar : enemyHealthBar_) {
		bar = std::make_unique<HealthBar>();
		bar->Init();
	}

	offScreen_ = std::make_unique<PostEffect::OffScreenRenderer>();
	offScreen_->Init();

	fullScreen_ = PostEffect::FullScreenRenderer::GetInstance();
	fullScreen_->Init({ L"FullScreen.PS.hlsl", L"GrayScale.PS.hlsl", L"Vignetting.PS.hlsl",  L"Smoothing.PS.hlsl", L"GaussianFilter.PS.hlsl" ,  L"GaussianFilterLiner.PS.hlsl",  L"HsvFillter.PS.hlsl" });

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "check.json" });

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, &newWorld_);

	levelDataManager->Destory(levelData);

	gaussianParam_->first = 32.f;
	gaussianParam_->second = 1;
	menuTimer_.Start(0.01f);

	vignettingParam_ = { 16.f, 0.8f };

	ECS::IFunctionalSystem::world_ = &newWorld_;

	ECS::System::Par::DrawEnemyHelthBar::Init(&enemyHealthBar_);

	// 生存などのデータの確認
	systemExecuter_.AddSystem<ECS::System::Par::CheckAliveTime>();
	systemExecuter_.AddSystem<ECS::System::Par::CheckHealthDie>();
	// 時間に関するもの
	systemExecuter_.AddSystem<ECS::System::Par::AddAliveTime>();
	systemExecuter_.AddSystem<ECS::System::Par::AddCoolTime>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcInvincibleTime>();
	// アニメーションなど
	systemExecuter_.AddSystem<ECS::System::Par::AnimateUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::ModelAnimatorUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::SkinModelUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::ColorLerp>();
	// 座標などの移動
	systemExecuter_.AddSystem<ECS::System::Par::AddGravity>();
	systemExecuter_.AddSystem<ECS::System::Par::AirResistanceSystem>();
	systemExecuter_.AddSystem<ECS::System::Par::MovePosition>();
	systemExecuter_.AddSystem<ECS::System::Par::MoveCollisionPosition>();

	// ゲーム固有の処理
	systemExecuter_.AddSystem<ECS::System::Par::EnemyMove>();
	systemExecuter_.AddSystem<ECS::System::Par::FallCollision>(); ECS::System::Par::FallCollision::ground_ = &ground_;
	systemExecuter_.AddSystem<ECS::System::Par::WeaponCollision>();
	systemExecuter_.AddSystem<ECS::System::Par::PlayerMove>();
	systemExecuter_.AddSystem<ECS::System::Par::PlayerAttack>(); ECS::System::Par::PlayerAttack::attackModel_ = attackModel_;
	systemExecuter_.AddSystem<ECS::System::Par::PlayerShooterUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::EnemyAttack>();
	systemExecuter_.AddSystem<ECS::System::Par::DamageUpdate>();

	// 汎用的な処理
	systemExecuter_.AddSystem<ECS::System::Par::SlideFollowCameraUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcEulerTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcParentTransform>();
	systemExecuter_.AddSystem<ECS::System::Par::ModelDrawer>();
	systemExecuter_.AddSystem<ECS::System::Par::SkinModelDrawer>();
	systemExecuter_.AddSystem<ECS::System::Par::DrawHelthBar>(); ECS::System::Par::DrawHelthBar::healthBar_ = healthBar_.get();
	systemExecuter_.AddSystem<ECS::System::Par::DrawEnemyHelthBar>();
	ECS::System::Par::DrawEnemyHelthBar::healthBar_ = &enemyHealthBar_;
	systemExecuter_.AddSystem<ECS::System::Par::CursorDrawer>();
	systemExecuter_.AddSystem<ECS::System::Par::ExpGaugeDrawer>();
	ECS::System::Par::ExpGaugeDrawer::expBar_ = expBar_.get();
	ECS::System::Par::ExpGaugeDrawer::prevLevel_ = 0u;

	levelUpUI_ = std::make_unique<LevelUP>();
	const Vector2 windowSize{ 320, 512 };
	const float windowDistance = 64.f;
	levelUpUI_->SetWindow(Vector2{ WinApp::kWindowWidth / 2.f, WinApp::kWindowHeight / 2.f }, windowSize, windowDistance);
	ECS::System::Par::ExpGaugeDrawer::levelUp_ = levelUpUI_.get();

	constexpr int32_t kUiCount = 3;
	levelUpUI_->Init(kUiCount);
	{
		ButtonUI *button = levelUpUI_->GetButtonUI(0);

		button->Init(TextureManager::Load("UI/RangeUp.png"), [this]() {

			captureRange_ += captureRangeLevelUp_;

			}
		);
	}

	{
		ButtonUI *button = levelUpUI_->GetButtonUI(1);

		button->Init(TextureManager::Load("UI/Shuriken.png"), [this]() {
			Archetype playerArchetype;
			playerArchetype.AddClassData<ECS::PlayerTag>();

			// プレイヤのView
			auto playerChunks = newWorld_.GetAccessableChunk(playerArchetype);

			for (auto chunk : playerChunks) {
				for (auto &player : *chunk) {
					auto &shooter = player.GetComponent<ECS::ArrowShooter>();
					// 弾追加
					shooter.count_++;
				}
			}
			}
		);
	}

	{
		ButtonUI *button = levelUpUI_->GetButtonUI(2);
		static constexpr int32_t powerUp = 5;

		button->Init(TextureManager::Load("UI/PowerUp.png"), [this]() {
			Archetype playerArchetype = Archetype::Generate<ECS::PlayerTag>();

			// プレイヤのView
			auto playerChunks = newWorld_.GetAccessableChunk(playerArchetype);

			for (auto chunk : playerChunks) {
				for (auto &player : *chunk) {
					auto &power = player.GetComponent<ECS::AttackPower>();
					// 攻撃力の増加
					power.power_ += powerUp;
				}
			}
			}
		);
	}

	gameTimer_.Start();
	for (uint32_t i = 0; i < 2; i++) {
		auto &timeUI = gameTimerUI_[i];

		timeUI = SolEngine::NumberText::Generate(TextureManager::Load("UI/Number.png"), 2u);
		timeUI->SetPosition(Vector2{ WinApp::kWindowWidth / 2.f + (i == 0 ? -72.f : 72.f),96.f });
		timeUI->SetPivot(Vector2{ static_cast<float>(1 - i), 1.f });

	}

	//timerCoron_ = Sprite::Generate();

	killUI_ = SolEngine::NumberText::Generate(TextureManager::Load("UI/Number.png"), 4);
	killUI_->SetPosition(Vector2{ static_cast<float>(WinApp::kWindowWidth) , 0 } + Vector2{ -96 * 2, (-vExpUICentorDiff_->y) * 8 });

	killUI_->SetPivot(Vector2::one * 0.5f);

}

void GameScene::OnExit() {
	audio_->StopAllWave();

	ECS::System::Par::WeaponCollision::attackCollisions_.reset();
	SetGameScore();
	auto nextScene = sceneManager_->GetNextScene<ResultScene>();
	if (nextScene) {
		nextScene->SetGameScore(GetGameScore());
	}

}

void GameScene::Update() {

	static bool skeletonDraw = false;
	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	[[maybe_unused]] const float fixDeltaTime = not (isMenuOpen_ or levelUpUI_->IsActive()) ? deltaTime : 0.f;
	[[maybe_unused]] const float powDeltaTime = fixDeltaTime * fixDeltaTime;

	ImGui::Text("XInput左スティックで移動");
	ImGui::Text("エンティティ数 / %lu", newWorld_.size());
	if (ImGui::Button("SkeletonDebug")) {
		skeletonDraw = not skeletonDraw;
	}

	light_->ImGuiWidget();

	damageTimer_.Update(deltaTime);
	gameScore_.aliveTime_ += fixDeltaTime;

	FlameClear();

	spawnTimer_.Update(fixDeltaTime);
	playerSpawn_.Update(fixDeltaTime);

	gameTimer_.Update(fixDeltaTime);
	{
		auto [m, s] = SoLib::Time::GetMoment(gameTimer_.GetTimeRemain());
		gameTimerUI_[0]->SetText(m);
		gameTimerUI_[1]->SetText(s);
	}

	// エンティティの追加
	spawner_.Execute(&newWorld_);
	// プレハブの破棄
	spawner_.clear();

	// 敵の追加
	AddSpawner(spawnTimer_, spawner_);

	// プレイヤの死亡処理
	PlayerDead(newWorld_, playerSpawn_, sceneManager_, Fade::GetInstance());

	// 経験値オーブの追加
	GenerateExperience(newWorld_, killCount_);

	// 攻撃の追加
	GeneratePlayerRangeAttack(newWorld_);
	// 飛び道具の追加
	GeneratePlayerArrowAttack(newWorld_);

	// もし生存フラグが折れていたら、配列から削除
	newWorld_.erase_if<ECS::IsAlive>([](const auto &item) {return not item.isAlive_; });

	// 経験値の加算
	PlayerExperience(newWorld_);
	// 描画のカウントのリセット
	ECS::System::Par::DrawEnemyHelthBar::drawCount_ = {};
	// ECSの処理の更新
	systemExecuter_.Execute(&newWorld_, fixDeltaTime);
	// カメラのアップデート
	cameraManager_->Update(fixDeltaTime);

	killUI_->SetText(static_cast<uint32_t>(killCount_));

	// 敵の描画
	ghostRenderer_.AddMatData<ECS::GhostModel>(newWorld_);
	shadowRenderer_.AddMatData<ECS::HasShadow>(newWorld_, shadowColor_, [](Particle::ParticleData &data) { Vector3 translate = data.transform.World.GetTranslate();
	data.transform.World = Matrix4x4::Identity();
	data.transform.World.GetTranslate() = Vector3{ translate.x, 0.1f, translate.z };
		}
	);

	// 経験値の描画
	expRender_.AddTransData<ECS::ExpOrb>(newWorld_, expColor_, [](Particle::ParticleData &data) { Vector3 translate = data.transform.World.GetTranslate();
	data.transform.World = Matrix4x4::Identity();
	data.transform.World.vecs[3] = { translate.x, 0.1f, translate.z, 1.f };
		}
	);

	// 攻撃範囲の描画
	AttackEffectRender(newWorld_, attackRender_);

	ArrowAttackEffectRender(newWorld_, arrowAttackRender_);

	const auto &camera = *cameraManager_->GetUseCamera();
	DamageRender(newWorld_, camera, *numberRender_);

	{
		// 入力処理
		levelUpUI_->InputFunc();
		levelUpUI_->Update(deltaTime);
	}


	if (skeletonDraw) {
		Archetype archetype;
		archetype.AddClassData<ECS::SkinModel, ECS::TransformMatComp>();
		auto chunks = newWorld_.GetAccessableChunk(archetype);
		for (auto &chunk : chunks) {
			for (auto [skin, trans] : chunk->View<ECS::SkinModel, ECS::TransformMatComp>()) {
				skin.skinModel_->skeleton_->AddDrawBuffer(trans, { .z = -2.f });
			}
		}
	}

	grayScaleParam_ = 1 - damageTimer_.GetProgress();

	ImGui::DragFloat2("VignettingParam", &vignettingParam_->first);

	ImGui::DragFloat("Sigma", &gaussianParam_->first);
	ImGui::DragInt("Size", &gaussianParam_->second);

	menuTimer_.Update(deltaTime);
	if (menuTimer_.IsFinish() and input_->GetDirectInput()->IsTrigger(DIK_ESCAPE)) {
		menuTimer_.Start(0.1f);

		isMenuOpen_ = not isMenuOpen_;
	}

	gaussianParam_->second = SoLib::Lerp(1, 32, isMenuOpen_ ? menuTimer_.GetProgress() : 1.f - menuTimer_.GetProgress());

	auto material = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance()->ImGuiWidget("MaterialManager");
	if (material) {
		SoLib::ImGuiWidget("Material", *material);
	}

	SoLib::ImGuiWidget("HsvParam", hsvParam_.get());
}

void GameScene::Draw() {

	const auto &camera = *cameraManager_->GetUseCamera();

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

	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);

	blockRender_->Draw(camera);
	modelHandleRender_->Draw(camera);

	ghostRenderer_.DrawExecute(camera);

	Model::SetPipelineType(Model::PipelineType::kSkinParticle);
	light_->SetLight(commandList);

	skinModelRender_->Draw(camera);
	skinModelHandleRender_->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kModel);
	light_->SetLight(commandList);

	Model::SetPipelineType(Model::PipelineType::kParticle);
	light_->SetLight(commandList);

	shadowRenderer_.DrawExecute(camera);
	attackRender_.DrawExecute(camera);
	expRender_.DrawExecute(camera);

	Model::SetPipelineType(Model::PipelineType::kShadowParticle);
	arrowAttackRender_.DrawExecute(camera);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	for (uint32_t count = 0; const auto & bar : enemyHealthBar_) {
		// もし描画数が超えた場合終了
		if (count >= ECS::System::Par::DrawEnemyHelthBar::drawCount_) { break; }
		bar->Draw();
		count++;
	}
	killUI_->Draw();

	numberRender_->Draw();

	healthBar_->Draw();

	expBar_->Draw();

	for (const auto &ui : gameTimerUI_) {
		ui->Draw();
	}
	// レベルアップの選択処理の描画
	levelUpUI_->Draw();

	// スプライトの描画
	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}

void GameScene::PostEffectSetup()
{
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDxCommon_->GetDsvDescHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = offScreen_->GetRtvDescHeap()->GetHeap()->GetCPUDescriptorHandleForHeapStart();

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

	pDxCommon_->DrawTargetReset(&rtvHandle, offScreen_->GetClearColor(), &dsvHandle, viewport, scissorRect);


}

void GameScene::PostEffectEnd()
{

	auto *const postEffectProcessor = PostEffect::ShaderEffectProcessor::GetInstance();

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

	auto resultTex = texStrage_->Allocate();

	// ポストエフェクトの初期値
	postEffectProcessor->Input(offScreen_->GetResource());

	// ガウスぼかし
	if (gaussianParam_->second > 1) {
		// 処理の実行
		postEffectProcessor->Execute(L"GaussianFilterLiner.PS.hlsl", gaussianParam_);
		postEffectProcessor->Execute(L"GaussianFilter.PS.hlsl", gaussianParam_);
	}

	//postEffectProcessor->Execute(L"Vignetting.PS.hlsl", vignettingParam_);

	//if (*grayScaleParam_.get() != 0) {
	//	postEffectProcessor->Execute(L"GrayScale.PS.hlsl", grayScaleParam_);
	//}
	//postEffectProcessor->Execute(L"HsvFillter.PS.hlsl", hsvParam_);

	// 結果を取り出す
	postEffectProcessor->GetResult(resultTex->renderTargetTexture_.Get());


	pDxCommon_->DefaultDrawReset(false);

	fullScreen_->Draw({ L"FullScreen.PS.hlsl" }, resultTex->renderTargetTexture_.Get(), resultTex->srvHandle_.gpuHandle_);

}

void GameScene::SetGameScore()
{
	gameScore_.killCount_ = killCount_;
}

const GameScore &GameScene::GetGameScore() const
{
	return gameScore_;
}

void GameScene::FlameClear()
{
	ghostRenderer_.Clear();
	shadowRenderer_.Clear();
	expRender_.Clear();
	attackRender_.Clear();
	arrowAttackRender_.Clear();
	particleArray_.clear();

	blockRender_->clear();
	skinModelRender_->clear();
	modelHandleRender_->clear();
	skinModelHandleRender_->clear();
	numberRender_->Clear();
}

void GameScene::PlayerDead(const ECS::World &world, SoLib::DeltaTimer &playerTimer, SolEngine::SceneManager *const scene, Fade *const fade) const
{
	Archetype playerArchetype;
	playerArchetype.AddClassData<ECS::PlayerTag>();
	{
		// プレイヤのView
		auto playerChunks = world.GetAccessableChunk(playerArchetype);

		// プレイヤのViewの長さが0である場合は死んでいる
		bool playerIsDead = playerChunks.Count() == 0u;

		bool isGameFinish = gameTimer_.IsFinish();

		// 死んでいた場合は
		if (playerIsDead or isGameFinish) {
			// スポーンタイマーが止まっていたら
			if (not playerTimer.IsActive()) {
				// 再度実行
				playerTimer.Start();
			}

			// 終わっていたら
			if ((playerTimer.IsFinish() and playerTimer.IsActive()) or isGameFinish) {

				scene->ChangeScene("ResultScene", 0.5f);

				fade->Start({}, 0x000000FF, 0.25f);
			}
		}
	}
}

void GameScene::GeneratePlayerArrowAttack(ECS::World &world) const
{
	// 攻撃しているプレイヤのアーキタイプ
	Archetype attackPlayerArch{};
	attackPlayerArch.AddClassData<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AttackPower, ECS::ArrowShooter>();
	// チャンクの取得
	const auto attackPlayerChunks = world.GetAccessableChunk(attackPlayerArch);
	// 撃ってる弾の数
	size_t fireCount = 0;
	// 攻撃中のプレイヤの数
	auto attackCount = attackPlayerChunks.CountIfFlag<ECS::ArrowShooter>([&fireCount](const ECS::ArrowShooter &flag) {
		bool isFire = flag.isFire_ and flag.count_;
		if (isFire) { fireCount += flag.count_; }
		return isFire;
		});
	// もし生成数が0なら終わり
	if (fireCount == 0) { return; }

	// 攻撃範囲のアーキタイプ
	Archetype areaArch;
	areaArch.AddClassData<ECS::SphereCollisionComp, ECS::AttackPower, ECS::KnockBackDirection, ECS::IsAlive, ECS::LifeLimit, ECS::AliveTime, ECS::AttackArrow, ECS::VelocityComp>();

	// 攻撃範囲の生成
	auto areaEntity = world.CreateEntity(areaArch, static_cast<uint32_t>(fireCount * 2 - 1)); // 書き込み先のIndex
	auto areaChanks = world.GetAccessableChunk(areaArch);

	auto areaView = areaEntity.View<ECS::SphereCollisionComp, ECS::AttackPower, ECS::KnockBackDirection, ECS::LifeLimit, ECS::VelocityComp>();

	auto playerPosRanges = attackPlayerChunks.GetRange<ECS::PositionComp>();
	auto playerRotRanges = attackPlayerChunks.GetRange<ECS::QuaternionRotComp>();
	auto playerAttackRanges = attackPlayerChunks.GetRange<ECS::ArrowShooter>();

	auto areaItr = areaView.begin();

	// プレイヤの数
	uint32_t size = attackPlayerChunks.Count();
	for (uint32_t i = 0; i < size; i++) {
		if (attackCount.first.at(i)) {
			const auto &attackStatus = playerAttackRanges.At(i);
			for (uint32_t c = 0; c < (attackStatus.count_ * 2 - 1); c++) {

				auto [sphere, attackPow, knockBack, lifeLim, velocity] = *areaItr++;

				const auto &playerFacing = playerRotRanges.At(i).quateRot_.GetFront();

				sphere.collision_.centor = playerPosRanges.At(i).position_;
				sphere.collision_.radius = attackStatus.radius_ * 0.25f;
				// 吹き飛ばす力
				knockBack.diffPower_ = { knockBackPower_, knockBackPower_ };
				knockBack.diff_ = Vector2{ playerFacing.x, playerFacing.z }.Nomalize();
				// 攻撃持続時間
				lifeLim.lifeLimit_ = 5.f;
				// 攻撃力
				attackPow.power_ = attackStatus.power_;

				float progress = (c + 0.5f) / static_cast<float>(attackStatus.count_ * 2 - 1);
				// 弾の方向の設定
				velocity.velocity_ = playerFacing * Quaternion::Create({ 0.f,attackStatus.angle_.Get() * (progress * 2 - 1) ,0.f }) * 20.f;
			}
		}
	}
}

void GameScene::GeneratePlayerRangeAttack(ECS::World &world) const
{
	// 攻撃しているプレイヤのアーキタイプ
	Archetype attackPlayerArch{};
	attackPlayerArch.AddClassData<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AttackPower, ECS::AttackStatus, ECS::AttackCooltime, ECS::AttackCollisionComp>();
	// チャンクの取得
	const auto attackPlayerChunks = world.GetAccessableChunk(attackPlayerArch);
	// 攻撃中のプレイヤの数
	auto attackCount = attackPlayerChunks.CountIfFlag<ECS::AttackCollisionComp>([](const ECS::AttackCollisionComp &flag) { return flag.isActive_; });
	// もし生成数が0なら終わり
	if (attackCount.second == 0) { return; }

	attackSound_.Play(false, 0.5f);

	// 攻撃範囲のアーキタイプ
	Archetype areaArch;
	areaArch.AddClassData<ECS::SphereCollisionComp, ECS::AttackPower, ECS::KnockBackDirection, ECS::IsAlive, ECS::LifeLimit, ECS::AliveTime, ECS::AttackRangeCircle>();

	// 攻撃範囲の生成
	uint32_t index = world.CreateEntity(areaArch, static_cast<uint32_t>(attackCount.second)).begin(); // 書き込み先のIndex
	auto areaChanks = world.GetAccessableChunk(areaArch);

	auto sphereRanges = areaChanks.GetRange<ECS::SphereCollisionComp>();
	auto attackPowerRanges = areaChanks.GetRange<ECS::AttackPower>();
	auto knockBackRanges = areaChanks.GetRange<ECS::KnockBackDirection>();
	auto lifeRanges = areaChanks.GetRange<ECS::LifeLimit>();

	auto playerPosRanges = attackPlayerChunks.GetRange<ECS::PositionComp>();
	auto playerRotRanges = attackPlayerChunks.GetRange<ECS::QuaternionRotComp>();
	auto playerAttackRanges = attackPlayerChunks.GetRange<ECS::AttackStatus>();
	auto playerPowerRanges = attackPlayerChunks.GetRange<ECS::AttackPower>();
	uint32_t size = attackPlayerChunks.Count();
	for (uint32_t i = 0; i < size; i++) {
		if (attackCount.first.at(i)) {
			const auto &attackStatus = playerAttackRanges.At(i);
			auto &sphere = sphereRanges.At(index);
			sphere.collision_.centor = playerPosRanges.At(i).position_ + attackStatus.offset_ * playerRotRanges.At(i).quateRot_.GetFront();
			sphere.collision_.radius = attackStatus.radius_;
			auto &knockBack = knockBackRanges.At(index);
			// 吹き飛ばす力
			knockBack.diffPower_ = { knockBackPower_, knockBackPower_ };
			// 攻撃持続時間
			lifeRanges.At(index).lifeLimit_ = attackTime_;
			// 攻撃力
			attackPowerRanges.At(index) = playerPowerRanges.At(i);

			// 次に移動
			++index;
		}
	}
}

void GameScene::GenerateExperience(ECS::World &world, size_t &killCount) const
{
	// 経験値の追加
	{
		// 敵のアーキタイプ
		Archetype enemArch{};
		enemArch.AddClassData<ECS::IsAlive, ECS::EnemyTag>();
		// チャンクの取得
		auto enemyChunks = world.GetAccessableChunk(enemArch);
		// 死亡している数
		auto deadCount = enemyChunks.CountIfFlag(ECS::IsAlive{ .isAlive_ = false });

		if (deadCount.second == 0) { return; }

		killCount += deadCount.second;

		// 経験値のアーキタイプ
		Archetype expArch;
		expArch.AddClassData<ECS::ExpOrb, ECS::PositionComp, ECS::IsAlive>();
		// 経験値オーブの生成
		auto ent = world.CreateEntity(expArch, static_cast<uint32_t>(deadCount.second));
		auto entItr = ent.View<ECS::PositionComp, ECS::IsAlive>().begin();

		auto enemRange = enemyChunks.GetRange<ECS::PositionComp>();
		uint32_t size = enemyChunks.Count();
		for (uint32_t i = 0; i < size; i++) {
			if (deadCount.first.at(i)) {
				auto [a, b] = (*entItr++);
				const auto &enemPos = enemRange.At(i);
				a = enemPos;
			}
		}
	}
}

void GameScene::PlayerExperience(ECS::World &world) const
{

	Archetype playerArchetype;
	playerArchetype.AddClassData<ECS::PlayerTag>();
	// プレイヤのView
	auto playerChunks = world.GetAccessableChunk(playerArchetype);
	if (playerChunks.Count()) {

		const auto &playerPos = playerChunks.GetRange<ECS::PositionComp>().At(0);
		auto &playerExp = playerChunks.GetRange<ECS::Experience>().At(0);

		// 経験値のアーキタイプ
		Archetype expArch;
		expArch.AddClassData<ECS::ExpOrb, ECS::PositionComp, ECS::IsAlive>();
		auto expChunks = world.GetAccessableChunk(expArch);

		const uint32_t size = expChunks.Count();

		auto expPosRange = expChunks.GetRange<ECS::PositionComp>();
		auto expAliveRange = expChunks.GetRange<ECS::IsAlive>();

		for (uint32_t i = 0; i < size; i++) {

			auto &isAlive = expAliveRange.At(i);
			// 生きてたら
			if (isAlive.isAlive_) {
				Vector3 pos = expPosRange.At(i).position_;
				pos.y = 0.f;

				// captureRange_以下なら吸収する
				if ((playerPos.position_ - pos).LengthSQ() <= captureRange_ * captureRange_) {
					playerExp.exp_++;
					isAlive.isAlive_ = false;
				}
			}
		}
	}
}

void GameScene::AttackEffectRender(const ECS::World &world, SolEngine::ModelInstancingRender &attackRender) const
{
	Archetype archetype;
	archetype.AddClassData<ECS::AttackRangeCircle>();
	// チャンクの取得
	auto ghostChanks = world.GetAccessableChunk(archetype);
	// オブジェクトの総数
	uint32_t totalCount = 0u;

	// チャンクと同じ数のデータを確保する
	std::vector<uint32_t> ghostOffset(ghostChanks.size());
	for (uint32_t i = 0; i < ghostOffset.size(); i++) {
		ghostOffset[i] = totalCount;
		totalCount += ghostChanks[i]->size();
	}
	// もし空なら終わる
	if (totalCount == 0) { return; }

	// 書き込み先の確保
	auto span = attackRender.Reservation(totalCount);
	for (uint32_t i = 0; i < ghostOffset.size(); i++) {
		// チャンクからデータの取得
		auto range = ghostChanks[i]->View<ECS::SphereCollisionComp, ECS::AliveTime>();
		// データを転送する
		const float attackTime = attackTime_;
		// 転送する
		std::transform(range.begin(), range.end(), &span[ghostOffset[i]], [attackTime](const auto &itm) {
			const auto &[trans, alive] = itm;

			Particle::ParticleData result{ .color = (0xFFFFFF00 + static_cast<uint32_t>(0xFF * (1 - SoLib::easeInExpo(alive.aliveTime_ / attackTime)))) };
			result.transform.World = Matrix4x4::AnyAngleRotate(Vector3::up, -SoLib::Angle::Rad360 * 2.f * SoLib::easeInOutBack(alive.aliveTime_ / attackTime)) * trans.collision_.radius * SoLib::easeOutExpo(alive.aliveTime_ / attackTime);
			result.transform.World.GetTranslate() = trans.collision_.centor;
			result.transform.World.m[3][3] = 1.f;
			return result;
			});
	}

}

void GameScene::ArrowAttackEffectRender(const ECS::World &world, SolEngine::ModelInstancingRender &attackRender) const
{
	Archetype archetype = Archetype::Generate<ECS::AttackArrow>();
	// チャンクの取得
	auto ghostChanks = world.GetAccessableChunk(archetype);
	// オブジェクトの総数
	uint32_t totalCount = 0u;

	// チャンクと同じ数のデータを確保する
	std::vector<uint32_t> ghostOffset(ghostChanks.size());
	for (uint32_t i = 0; i < ghostOffset.size(); i++) {
		ghostOffset[i] = totalCount;
		totalCount += ghostChanks[i]->size();
	}
	// もし空なら終わる
	if (totalCount == 0) { return; }

	// 書き込み先の確保
	auto span = attackRender.Reservation(totalCount);
	for (uint32_t i = 0; i < ghostOffset.size(); i++) {
		// チャンクからデータの取得
		auto range = ghostChanks[i]->View<ECS::SphereCollisionComp, ECS::AliveTime>();
		// データを転送する
		const float attackRotSpeed = arrowAttackRotSpeed_;
		// 転送する
		std::transform(range.begin(), range.end(), &span[ghostOffset[i]], [attackRotSpeed](const auto &itm) {
			const auto &[trans, alive] = itm;

			Particle::ParticleData result{ .color = 0x333333FF };
			result.transform.World = Matrix4x4::AnyAngleRotate(Vector3::up, SoLib::Angle::Rad360 * alive.aliveTime_ / attackRotSpeed) * (trans.collision_.radius * 0.25f);
			result.transform.World.GetTranslate() = trans.collision_.centor;
			result.transform.World.m[3][3] = 1.f;
			return result;
			});

	}
}

void GameScene::AddSpawner(SoLib::DeltaTimer &timer, ECS::Spawner &spawner) const
{

	if (timer.IsFinish()) {

		const float gameProgress = gameTimer_.GetProgress();

		const int32_t enemyHealth = static_cast<int32_t>(*vEnemyHealthBase_ + *vEnemyHealthDiff_ * gameProgress);
		// 敵のスポーン数
		const int32_t enemyCount = *vEnemySpawnCount_;
		// 敵の沸く半径
		const float enemyRadius = *vEnemyRadius_;

		// スポナーに追加を要求する
		spawner.AddSpawner(enemyPrefab_.get(), enemyCount, [enemyCount, enemyRadius, enemyHealth, gameProgress](const ECS::EntityList<false> &enemys)
			{
				// コンポーネントの配列
				auto arr = enemys.View<ECS::PositionComp, ECS::HealthComp>();
				// 発生地点の回転加算値
				const float diff = SoLib::Random::GetRandom<float>(0.f, SoLib::Angle::Rad360);
				for (uint32_t i = 0; i < enemys.ItrRange().size(); i++) {
					auto [pos, health] = *(arr.begin() + i);
					pos.position_ = SoLib::EulerToDirection(SoLib::Euler{ 0.f, (SoLib::Angle::Rad360 / enemyCount) * i + diff, 0.f }) * enemyRadius;
					health = ECS::HealthComp::Create(enemyHealth);
				}
			});
		timer.Start();
	}
}

void GameScene::DamageRender([[maybe_unused]] const ECS::World &world, const SolEngine::Camera3D &camera, [[maybe_unused]] SolEngine::NumberRender &numberRender) const
{
	auto chunks = world.GetAccessableChunk(Archetype::Generate<ECS::DamageCounter>());
	size_t totalCount = 0;

	// チャンクと同じ数のデータを確保する
	std::vector<std::tuple<size_t, std::vector<bool>, size_t>> chunkOffset(chunks.size());
	for (uint32_t i = 0; i < chunkOffset.size(); i++) {
		auto &[index, flag, count] = chunkOffset[i];
		index = totalCount;
		auto [rFlag, rCount] = chunks[i]->CountIfFlag<ECS::DamageCounter>([](const ECS::DamageCounter &flag) {
			return (flag.damageCount_ != 0 and flag.damageRemainTime_ > 0);
			});
		flag = std::move(rFlag);
		count = rCount;
		totalCount += count;

	}
	if (totalCount == 0) {
		return;
	}

	// 書き込み先のメモリの確保
	auto numberSpan = numberRender.Reservation(totalCount);

	const Matrix4x4 &vp = SolEngine::Render::MakeViewportMatrix({ 0,0 }, WinApp::kWindowWidth, WinApp::kWindowHeight);
	const Matrix4x4 &matVPVp = camera.matView_ * camera.matProjection_ * vp;

	for (size_t i = 0; i < chunkOffset.size(); i++) {
		const auto &[index, flag, count] = chunkOffset[i];
		const auto &chunk = chunks[i];

		auto numItr = &numberSpan[index];

		const auto &chunkView = chunk->View<ECS::PositionComp, ECS::DamageCounter>();
		auto viewItr = chunkView.begin();

		for (uint32_t j = 0; j < chunkView.size(); j++) {
			auto target = viewItr++;
			if (flag[j]) {
				const auto &[position, damageCounter] = *target;
				auto num = (*(numItr++)).get();

				num->SetText(damageCounter.damageCount_);

				num->SetPosition(SolEngine::Render::WorldToScreen(position, matVPVp).ToVec2());

			}
		}

	}


}

bool GameScene::ImGuiWidget(const std::string_view &name)
{
	if (ImGui::TreeNode(name.data())) {

		SoLib::ImGuiWidget(&this->vEnemyHealthBase_);
		SoLib::ImGuiWidget(&this->vEnemyHealthDiff_);

		SoLib::ImGuiWidget(&this->vEnemySpawnCount_);
		SoLib::ImGuiWidget(&this->vEnemyRadius_);

		SoLib::ImGuiWidget(&this->vLevelUpUISize_);

		SoLib::ImGuiWidget(&this->vExpUICentorMul_);
		SoLib::ImGuiWidget(&this->vExpUICentorDiff_);

		ImGui::TreePop();
	}
	return false;
}

void GameScene::Load(const GlobalVariables::Group &group)
{
	group >> vEnemyHealthBase_;
	group >> vEnemyHealthDiff_;

	group >> vEnemySpawnCount_;
	group >> vEnemyRadius_;

	group >> vLevelUpUISize_;

	group >> vExpUICentorMul_;
	group >> vExpUICentorDiff_;
}

void GameScene::Save(GlobalVariables::Group &group) const
{
	group << vEnemyHealthBase_;
	group << vEnemyHealthDiff_;

	group << vEnemySpawnCount_;
	group << vEnemyRadius_;

	group << vLevelUpUISize_;

	group << vExpUICentorMul_;
	group << vExpUICentorDiff_;
}
