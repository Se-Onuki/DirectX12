#include "GameScene.h"

#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../Header/Object/Fade.h"
#include "../Header/Object/Particle/ParticleEmitterManager.h"
#include "../Utils/SoLib/SoLib_ImGui.h"
#include "../Utils/SoLib/SoLib.h"
#include <imgui.h>
#include "TitleScene.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "../../Utils/Math/Angle.h"
#include "../ECS/System/Systems.h"
#include "../Engine/LevelEditor/LevelData.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../ECS/System/FunctionalSystem.h"
#include "../ECS/System/NewSystems.h"
#include "../Engine/DirectBase/Model/SkeletonAnimation/Skeleton.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
	blockRender_ = BlockManager::GetInstance();
	skinModelRender_ = SkinModelListManager::GetInstance();
	modelHandleRender_ = ModelHandleListManager::GetInstance();
	skinModelHandleRender_ = SkinModelHandleListManager::GetInstance();
	particleManager_ = ParticleManager::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {

	pDxCommon_ = DirectXCommon::GetInstance();
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader, ShaderSource>::GetInstance();
	texStrage_ = SolEngine::FullScreenTextureStrage::GetInstance();

	compRegistry_ = ECS::ComponentRegistry::GetInstance();

	SolEngine::ResourceObjectManager<SolEngine::AssimpData> *const assimpManager = SolEngine::ResourceObjectManager<SolEngine::AssimpData>::GetInstance();
	SolEngine::ResourceObjectManager<SolEngine::ModelData> *const modelDataManager = SolEngine::ResourceObjectManager<SolEngine::ModelData>::GetInstance();
	SolEngine::ResourceObjectManager<SolEngine::Skeleton> *const skeletonManager = SolEngine::ResourceObjectManager<SolEngine::Skeleton>::GetInstance();

	auto playerAssimp = assimpManager->Load({ "Model/human/", "sneakWalk.gltf" });
	auto playerModel = modelDataManager->Load({ playerAssimp });
	auto playerSkeleton = skeletonManager->Load({ playerAssimp });

	auto boxAssimp = assimpManager->Load({ "Model/Cute Animated Monsters Pack/", "Ghost.gltf" });
	auto boxModel = modelDataManager->Load({ boxAssimp });

	auto brainStemAssimp = assimpManager->Load({ "Model/human/", "BrainStem.glb" });
	brainStem_ = modelDataManager->Load({ brainStemAssimp });

	ECS::ComponentRegistry::ComponentFlag compFlag = compRegistry_->CreateFlag<ECS::IsAlive, ECS::TransformMatComp>();
	ECS::ComponentRegistry::ComponentFlag compBigFlag = compRegistry_->CreateFlag<ECS::IsAlive, ECS::TransformMatComp, ECS::PositionComp>();

	light_ = DirectionLight::Create();

	blockRender_->Init(1024u);
	skinModelRender_->Init(1024u);
	modelHandleRender_->Init(1024u);
	skinModelHandleRender_->Init(1024u);
	particleManager_->Init(2048u);

	ModelManager::GetInstance()->CreateDefaultModel();

	ModelManager::GetInstance()->GetModel("Plane")->materialMap_.begin()->second->texHandle_ = TextureManager::Load("uvChecker.png");

	spawner_.clear();

	auto animationManager = SolEngine::ResourceObjectManager<SolEngine::Animation>::GetInstance();

	animation_ = animationManager->Load(assimpManager->Load({ "Model/human/", "walk.gltf" }));
	attackAnimation_ = animationManager->Load({ assimpManager->Load({ "Model/human/", "Attack.gltf" }), 1 });

	skinModel_ = SkinModel::MakeSkinModel(*playerModel, *playerSkeleton);

	boxModel_ = ModelManager::GetInstance()->AddModel("Block", Model::LoadAssimpModelFile("", "box.obj"));
	model_ = ModelManager::GetInstance()->AddModel("Particle", Model::CreatePlane());
	model_->materialMap_.begin()->second->texHandle_ = TextureManager::Load("circle.png");
	model_->materialMap_.begin()->second->blendMode_ = Model::BlendMode::kAdd;

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

	/*Archetype archetype;
	archetype.AddClassData<ECS::Identifier, ECS::ModelComp, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::BillboardRotate, ECS::Color, ECS::VelocityComp>();

	mArray_ = std::make_unique<ECS::MultiArray>(archetype);

	Archetype emitterArchetype;
	emitterArchetype.AddClassData<ECS::Identifier, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::EmitterComp>();
	emitterArray_ = std::make_unique<ECS::MultiArray>(emitterArchetype);*/

	//Archetype particleArchetype;
	//particleArchetype.AddClassData<ECS::Identifier, ECS::ModelComp, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::BillboardRotate, ECS::Color, ECS::VelocityComp, ECS::ColorLarp>();

	//entityManager_->CreateEntity(particleArchetype, 3u);
	//auto emitterList = entityManager_->CreateEntity<ECS::Identifier, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::EmitterComp>();


	prefab_ = std::make_unique<ECS::Prefab>();

	*prefab_ += ECS::Identifier{};
	*prefab_ += ECS::IsAlive{};
	*prefab_ += ECS::PositionComp{};
	*prefab_ += ECS::RotateComp{};
	*prefab_ += ECS::ScaleComp{};
	*prefab_ += ECS::AliveTime{};
	*prefab_ += ECS::EmitterComp{ .count_ = 5u, .startColor_ = 0xFFFF00FF, .endColor_ = 0xFF000000, .spawnLifeLimit_{0.1f, 0.2f}, .spawnPower_{0.05f, 0.1f}, .spawnRange_{90._deg, 180._deg, 0.f} };

	//entityManager_->CreateEntity(*prefab_);
	newWorld_.CreateEntity(*prefab_);

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
	*playerPrefab_ += ECS::AttackCollisionComp{ };
	*playerPrefab_ += ECS::EntityState{};
	*playerPrefab_ += ECS::HealthComp::Create(120);
	*playerPrefab_ += ECS::InvincibleTime{ .timer_{ 1.f, false } };
	*playerPrefab_ += ECS::AirResistance{ .resistance = (3.6f / 60.f) };
	*playerPrefab_ += ECS::CursorComp{ .model_ = cursor, .inModel_ = inCursor };
	*playerPrefab_ += ECS::AttackStatus{ };
	*playerPrefab_ += ECS::AttackPower{ .power_ = 20 };
	*playerPrefab_ += ECS::AttackCooltime{ .cooltime_ = { 1.0f, false } };
	*playerPrefab_ += ECS::Experience{ };

	//entityManager_->CreateEntity(*playerPrefab_);
	newWorld_.CreateEntity(*playerPrefab_);

	enemyPrefab_ = std::make_unique<ECS::Prefab>();
	*enemyPrefab_ += ECS::IsAlive{};
	*enemyPrefab_ += ECS::ScaleComp{ };
	*enemyPrefab_ += ECS::QuaternionRotComp{};
	*enemyPrefab_ += ECS::PositionComp{ .position_{0.f, 1.f, 10.f} };
	*enemyPrefab_ += ECS::TransformMatComp{};
	*enemyPrefab_ += ECS::ModelComp{ .model_ = {boxModel} };
	*enemyPrefab_ += ECS::GravityComp{};
	*enemyPrefab_ += ECS::SphereCollisionComp{ .collision_ = Sphere{.radius = 1.f } };
	*enemyPrefab_ += ECS::EnemyTag{};
	*enemyPrefab_ += ECS::HealthComp::Create(100);
	*enemyPrefab_ += ECS::HealthBarComp{};
	*enemyPrefab_ += ECS::AttackPower{ .power_ = 10 };
	*enemyPrefab_ += ECS::AttackCooltime{ .cooltime_ = { 5.f, false } };

	//entityManager_->CreateEntity(*enemyPrefab_);
	newWorld_.CreateEntity(*enemyPrefab_);

	// カメラのオフセット位置
	const Vector3 cameraOffset{ 0.f,1.f,-1.f };

	ECS::Prefab followCamera;
	followCamera += ECS::FollowCamera{ .rotation_ = Quaternion::LookAt(-cameraOffset), .offset_ {.z = -40.f} };
	followCamera += ECS::PositionComp{};

	//entityManager_->CreateEntity(followCamera);
	newWorld_.CreateEntity(followCamera);

	soundA_ = audio_->LoadMP3("resources/Audio/SE/Player/startRotate1.mp3");

	ground_.Init();

	spawnTimer_.Start();
	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);

	healthBar_ = std::make_unique<HealthBar>();
	healthBar_->Init();

	expBar_ = std::make_unique<HealthBar>();
	expBar_->Init();

	expBar_->SetCentor({ static_cast<float>(WinApp::kWindowWidth) * 0.5f, static_cast<float>(WinApp::kWindowHeight) - 16.f });
	expBar_->SetScale({ static_cast<float>(WinApp::kWindowWidth), 32.f });

	levelUI_ = Sprite::Create();
	levelUI_->SetTextureHaundle(TextureManager::Load("UI/LevelUP.png"));
	levelUI_->SetPivot(Vector2::one * 0.5f);
	levelUI_->SetScale(Vector2{ 256.f,64.f } *2.f);
	levelUI_->SetPosition({ static_cast<float>(WinApp::kWindowWidth) * 0.5f, static_cast<float>(WinApp::kWindowHeight) - 16.f - 64.f * 2.f });

	for (auto &bar : enemyHealthBar_) {
		bar = std::make_unique<HealthBar>();
		bar->Init();
	}

	//// 生存などのデータの確認
	//systemManager_.AddSystem<ECS::System::CheckAliveTime>();
	//systemManager_.AddSystem<ECS::System::CheckHealthDie>();
	//// 時間に関するもの
	//systemManager_.AddSystem<ECS::System::AddAliveTime>();
	//systemManager_.AddSystem<ECS::System::AddCoolTime>();
	//// アニメーションなど
	//systemManager_.AddSystem<ECS::System::AnimateUpdate>();
	//systemManager_.AddSystem<ECS::System::ModelAnimatorUpdate>();
	//systemManager_.AddSystem<ECS::System::SkinModelUpdate>();
	//systemManager_.AddSystem<ECS::System::ColorLerp>();
	//// 座標などの移動
	//systemManager_.AddSystem<ECS::System::AddGravity>();
	//systemManager_.AddSystem<ECS::System::AirResistanceSystem>();
	//systemManager_.AddSystem<ECS::System::MovePosition>();

	//// ゲーム固有の処理
	//systemManager_.AddSystem<ECS::System::EnemyMove>();
	//systemManager_.AddSystem<ECS::System::FallCollision>(&ground_);
	//systemManager_.AddSystem<ECS::System::WeaponCollision>(soundA_, model_);
	//systemManager_.AddSystem<ECS::System::PlayerMove>();
	//systemManager_.AddSystem<ECS::System::PlayerAttack>(attackModel_);
	//systemManager_.AddSystem<ECS::System::EnemyAttack>([this]() { damageTimer_.Start(0.5f); });

	//// 汎用的な処理
	//systemManager_.AddSystem<ECS::System::SlideFollowCameraUpdate>();
	//systemManager_.AddSystem<ECS::System::CalcTransMatrix>();
	//systemManager_.AddSystem<ECS::System::CalcParentTransform>();
	//systemManager_.AddSystem<ECS::System::ModelDrawer>();
	//systemManager_.AddSystem<ECS::System::DrawHelthBar>(healthBar_.get());
	//systemManager_.AddSystem<ECS::System::DrawEnemyHelthBar>(&enemyHealthBar_);
	//systemManager_.AddSystem<ECS::System::CursorDrawer>();
	//systemManager_.AddSystem<ECS::System::ExpGaugeDrawer>(levelUI_.get(), expBar_.get());

	offScreen_ = std::make_unique<PostEffect::OffScreenRenderer>();
	offScreen_->Init();

	fullScreen_ = PostEffect::FullScreenRenderer::GetInstance();
	fullScreen_->Init({ { L"FullScreen.VS.hlsl",L"FullScreen.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"GrayScale.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"Vignetting.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"Smoothing.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"GaussianFilter.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"GaussianFilterLiner.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"HsvFillter.PS.hlsl" } });

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "check.json" });

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, &newWorld_);

	levelDataManager->Destory(levelData);

	gaussianParam_->first = 32.f;
	gaussianParam_->second = 1;
	menuTimer_.Start(0.01f);

	brainStemTrans_->scale = Vector3::one * 5.f;
	brainStemTrans_->translate.z = -30.f;
	brainStemTrans_->UpdateMatrix();

	vignettingParam_ = { 16.f, 0.8f };

	ECS::IFunctionalSystem::world_ = &newWorld_;

	ECS::System::Par::DrawEnemyHelthBar::Init(&enemyHealthBar_);

	// 生存などのデータの確認
	systemExecuter_.AddSystem<ECS::System::Par::CheckAliveTime>();
	systemExecuter_.AddSystem<ECS::System::Par::CheckHealthDie>();
	// 時間に関するもの
	systemExecuter_.AddSystem<ECS::System::Par::AddAliveTime>();
	systemExecuter_.AddSystem<ECS::System::Par::AddCoolTime>();
	// アニメーションなど
	systemExecuter_.AddSystem<ECS::System::Par::AnimateUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::ModelAnimatorUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::SkinModelUpdate>();
	systemExecuter_.AddSystem<ECS::System::Par::ColorLerp>();
	// 座標などの移動
	systemExecuter_.AddSystem<ECS::System::Par::AddGravity>();
	systemExecuter_.AddSystem<ECS::System::Par::AirResistanceSystem>();
	systemExecuter_.AddSystem<ECS::System::Par::MovePosition>();

	// ゲーム固有の処理
	systemExecuter_.AddSystem<ECS::System::Par::EnemyMove>();
	systemExecuter_.AddSystem<ECS::System::Par::FallCollision>(); ECS::System::Par::FallCollision::ground_ = &ground_;
	systemExecuter_.AddSystem<ECS::System::Par::WeaponCollision>();
	systemExecuter_.AddSystem<ECS::System::Par::PlayerMove>();
	systemExecuter_.AddSystem<ECS::System::Par::PlayerAttack>(); ECS::System::Par::PlayerAttack::attackModel_ = attackModel_;
	systemExecuter_.AddSystem<ECS::System::Par::EnemyAttack>();

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
	//systemExecuter_.AddSystem<ECS::System::Par::ExpGaugeDrawer>(); ECS::System::Par::ExpGaugeDrawer::

}

void GameScene::OnExit() {
	audio_->StopAllWave();
	fullScreen_->Finalize();
}

void GameScene::Update() {

	static bool skeletonDraw = false;
	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	[[maybe_unused]] const float fixDeltaTime = not isMenuOpen_ ? deltaTime : 0.f;
	[[maybe_unused]] const float powDeltaTime = fixDeltaTime * fixDeltaTime;

	ImGui::Text("XInput左スティックで移動");
	ImGui::Text("エンティティ数 / %lu", newWorld_.size());
	if (ImGui::Button("SkeletonDebug")) {
		skeletonDraw = not skeletonDraw;
	}

	light_->ImGuiWidget();

	damageTimer_.Update(deltaTime);

	particleArray_.clear();

	blockRender_->clear();
	skinModelRender_->clear();
	modelHandleRender_->clear();
	skinModelHandleRender_->clear();

	spawnTimer_.Update(fixDeltaTime);
	playerSpawn_.Update(fixDeltaTime);

	// エンティティの追加
	spawner_.Execute(&newWorld_);
	// プレハブの破棄
	spawner_.clear();

	// 敵のスポーン数
	constexpr uint32_t kEnemyCount = 15u;
	// 敵の沸く半径
	constexpr float kEnemyRadius = 45.f;

	if (spawnTimer_.IsFinish()) {

		// スポナーに追加を要求する
		spawner_.AddSpawner(enemyPrefab_.get(), kEnemyCount, [](auto enemys)
			{
				// 発生地点の回転加算値
				const float diff = Random::GetRandom<float>(0.f, Angle::Rad360);
				for (uint32_t i = 0; ECS::EntityClass & enemy : enemys) {
					auto &pos = enemy.GetComponent<ECS::PositionComp>();
					pos.position_ = SoLib::EulerToDirection(SoLib::Euler{ 0.f, (Angle::Rad360 / kEnemyCount) * i + diff, 0.f }) * kEnemyRadius;
					i++;
				}
			});
		spawnTimer_.Start();
	}

	Archetype playerArchetype;
	playerArchetype.AddClassData<ECS::PlayerTag>();

	// プレイヤのView
	auto playerChunks = newWorld_.GetAccessableChunk(playerArchetype);

	// プレイヤのViewの長さが0である場合は死んでいる
	bool playerIsDead = playerChunks.Count() == 0u;

	// 死んでいた場合は
	if (playerIsDead) {
		// スポーンタイマーが止まっていたら
		if (not playerSpawn_.IsActive()) {
			// 再度実行
			playerSpawn_.Start();
		}

		// 終わっていたら
		if (playerSpawn_.IsFinish()) {
			// スポナーに追加
			spawner_.AddSpawner(playerPrefab_.get());
		}
	}


	// もし生存フラグが折れていたら、配列から削除
	newWorld_.erase_if<ECS::IsAlive>([](const auto &item) {return not item->isAlive_; });

	ECS::System::Par::DrawEnemyHelthBar::drawCount_ = 0u;

	systemExecuter_.Execute(&newWorld_, fixDeltaTime);

	cameraManager_->Update(fixDeltaTime);

	if (skeletonDraw) {
		Archetype archetype;
		archetype.AddClassData<ECS::SkinModel, ECS::TransformMatComp>();
		auto chunks = newWorld_.GetAccessableChunk(archetype);
		for (auto &chunk : chunks) {
			auto skinRange = chunk->GetComponent<ECS::SkinModel>();
			auto transRange = chunk->GetComponent<ECS::TransformMatComp>();
			for (uint32_t i = 0; i < skinRange.size(); i++) {
				skinRange[i].skinModel_->skeleton_->AddDrawBuffer(transRange[i], { .z = -2.f });
			}
		}
	}

	grayScaleParam_ = 1 - damageTimer_.GetProgress();

	ImGui::DragFloat2("VignettingParam", &vignettingParam_->first);

	//float health = 0.f;
	//for (const auto &[entity, healthComp, player] : world_->view<const ECS::HealthComp, const ECS::PlayerTag>()) {
	//	health = healthComp->CalcPercent();
	//}
	//vignettingParam_->first = SoLib::Lerp(0.5f, 16.f, health);

	//for (const auto &[entity, color, billboard, mat] : world_->view<const ECS::Color, const ECS::BillboardRotate, const ECS::TransformMatComp>()) {
	//	particleArray_.push_back(Particle::ParticleData{ .transform = mat->transformMat_, .color = color->color_ });
	//}

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

	particleManager_->Update(fixDeltaTime);
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

	Model::SetPipelineType(Model::PipelineType::kSkinParticle);
	light_->SetLight(commandList);

	skinModelRender_->Draw(camera);
	skinModelHandleRender_->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kModel);
	light_->SetLight(commandList);

	Model::SetPipelineType(Model::PipelineType::kParticle);

	light_->SetLight(commandList);

	model_->Draw(particleArray_, camera);

	particleManager_->Draw(camera);

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

	healthBar_->Draw();

	expBar_->Draw();
	//levelUI_->Draw();
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

	fullScreen_->Draw({ L"FullScreen.VS.hlsl",L"FullScreen.PS.hlsl" }, resultTex->renderTargetTexture_.Get(), resultTex->srvHandle_.gpuHandle_);

}
