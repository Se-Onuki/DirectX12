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

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
	blockRender_ = BlockManager::GetInstance();
	skinModelRender_ = SkinModelListManager::GetInstance();
	particleManager_ = ParticleManager::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	pDxCommon_ = DirectXCommon::GetInstance();
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader, ShaderSource>::GetInstance();

	compRegistry_ = ECS::ComponentRegistry::GetInstance();

	ECS::ComponentRegistry::ComponentFlag compFlag = compRegistry_->CreateFlag<ECS::IsAlive, ECS::TransformMatComp>();
	ECS::ComponentRegistry::ComponentFlag compBigFlag = compRegistry_->CreateFlag<ECS::IsAlive, ECS::TransformMatComp, ECS::PositionComp>();

	if ((compFlag.Get() & compBigFlag.Get()) == compFlag.Get()) {
		compFlag = compBigFlag;
	}

	light_ = DirectionLight::Create();

	blockRender_->Init(1024u);
	skinModelRender_->Init(1024u);
	particleManager_->Init(2048u);

	world_ = std::make_unique<World>();
	entityManager_ = world_->GetEntityManager();
	ModelManager::GetInstance()->CreateDefaultModel();

	ModelManager::GetInstance()->GetModel("Plane")->materialMap_.begin()->second->texHandle_ = TextureManager::Load("uvChecker.png");

	spawner_.clear();

	//assimpModel_ = Model::LoadAssimpObjFile("", "box.obj");

	playerModel_ = ModelManager::GetInstance()->AddModel("Pleyer", Model::LoadAssimpModelFile("Model/human/", "sneakWalk.gltf"));
	animation_ = ModelAnimation::Animation::CreateFromFile("Model/human/", "walk.gltf");
	attackAnimation_ = ModelAnimation::Animation::CreateFromFile("Model/human/", "Attack.gltf", 1);

	skinModel_ = SkinModel::MakeSkinModel(playerModel_);

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

	auto sphere = ModelManager::GetInstance()->AddModel("Sphere", Model::LoadObjFile("", "Sphere.obj"));

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

	entityManager_->CreateEntity(*prefab_);

	playerPrefab_ = std::make_unique<ECS::Prefab>();

	*playerPrefab_ += ECS::IsAlive{};
	*playerPrefab_ += ECS::ScaleComp{ .scale_ = Vector3::one * 2.5f };
	*playerPrefab_ += ECS::QuaternionRotComp{};
	*playerPrefab_ += ECS::PositionComp{};
	*playerPrefab_ += ECS::InputFlagComp{};
	*playerPrefab_ += ECS::TransformMatComp{};
	*playerPrefab_ += ECS::ModelAnimator{ .animateList_{{ &animation_, &animation_, &attackAnimation_, &attackAnimation_}},.animatior_ = &animation_ };
	*playerPrefab_ += ECS::SkinModel{ .skinModel_ = skinModel_.get() };
	*playerPrefab_ += ECS::ModelComp{ .model_ = playerModel_ };
	//*playerPrefab_ += ECS::BoneTransformComp{ .boneTransform_{{BoneModel::SimpleTransform{},BoneModel::SimpleTransform{.translate_{0.f,1.f,0.f}}}} };
	*playerPrefab_ += ECS::VelocityComp{};
	*playerPrefab_ += ECS::AccelerationComp{};
	*playerPrefab_ += ECS::GravityComp{ .gravity_ = Vector3::up * -9.8f };
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

	entityManager_->CreateEntity(*playerPrefab_);

	enemyPrefab_ = std::make_unique<ECS::Prefab>();
	*enemyPrefab_ += ECS::IsAlive{};
	*enemyPrefab_ += ECS::ScaleComp{ };
	*enemyPrefab_ += ECS::QuaternionRotComp{};
	*enemyPrefab_ += ECS::PositionComp{ .position_{0.f, 1.f, 10.f} };
	*enemyPrefab_ += ECS::TransformMatComp{};
	*enemyPrefab_ += ECS::ModelComp{ .model_{ModelManager::GetInstance()->GetModel("Block")} };
	*enemyPrefab_ += ECS::GravityComp{ .gravity_ = Vector3::up * -9.8f };
	*enemyPrefab_ += ECS::SphereCollisionComp{ .collision_ = Sphere{.radius = 1.f } };
	*enemyPrefab_ += ECS::EnemyTag{};
	*enemyPrefab_ += ECS::HealthComp::Create(100);
	*enemyPrefab_ += ECS::HealthBarComp{};
	*enemyPrefab_ += ECS::AttackPower{ .power_ = 10 };
	*enemyPrefab_ += ECS::AttackCooltime{ .cooltime_ = { 5.f, false } };

	entityManager_->CreateEntity(*enemyPrefab_);

	// カメラのオフセット位置
	const Vector3 cameraOffset{ 0.f,1.f,-1.f };

	ECS::Prefab followCamera;
	followCamera += ECS::FollowCamera{ .rotation_ = Quaternion::LookAt(-cameraOffset), .offset_ {.z = -40.f} };
	followCamera += ECS::PositionComp{};

	entityManager_->CreateEntity(followCamera);

	//*enemyPrefab_ += ECS::RotateComp{};

	soundA_ = audio_->LoadMP3("resources/Audio/SE/Player/startRotate1.mp3");

	boneModel_.Init();
	auto *bodyPtr = boneModel_.AddBone("Body", boxModel_);
	boneModel_.AddBone("Head", boxModel_, bodyPtr);
	boneModel_.AddBone("Sword", boxModel_, bodyPtr);

	boneModel_.SetNumber();

	ground_.Init();

	spawnTimer_.Start();
	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);

	{
		gameObject_.Init();
		auto modelComp = gameObject_.AddComponent<ModelComp>();
		modelComp->AddBone("Body", sphere);

		gameObject_.transform_.translate = { 0.f,3.f,0.f };
	}

	healthBar_ = std::make_unique<HealthBar>();
	healthBar_->Init();

	for (auto &bar : enemyHealthBar_) {
		bar = std::make_unique<HealthBar>();
		bar->Init();
	}

	// 生存などのデータの確認
	systemManager_.AddSystem<ECS::System::CheckAliveTime>();
	systemManager_.AddSystem<ECS::System::CheckHealthDie>();
	// 時間に関するもの
	systemManager_.AddSystem<ECS::System::AddAliveTime>();
	systemManager_.AddSystem<ECS::System::AddCoolTime>();
	// アニメーションなど
	systemManager_.AddSystem<ECS::System::AnimateUpdate>();
	systemManager_.AddSystem<ECS::System::ModelAnimatorUpdate>();
	systemManager_.AddSystem<ECS::System::SkinModelUpdate>();
	systemManager_.AddSystem<ECS::System::ColorLerp>();
	// 座標などの移動
	systemManager_.AddSystem<ECS::System::AddGravity>();
	systemManager_.AddSystem<ECS::System::AirResistanceSystem>();
	systemManager_.AddSystem<ECS::System::MovePosition>();

	// ゲーム固有の処理
	systemManager_.AddSystem<ECS::System::EnemyMove>();
	systemManager_.AddSystem<ECS::System::FallCollision>(&ground_);
	systemManager_.AddSystem<ECS::System::WeaponCollision>(soundA_, model_);
	systemManager_.AddSystem<ECS::System::PlayerMove>();
	systemManager_.AddSystem<ECS::System::PlayerAttack>(attackModel_);
	systemManager_.AddSystem<ECS::System::EnemyAttack>([this]() { isGrayScale_ = 30; });

	// 汎用的な処理
	//systemManager_.AddSystem<ECS::System::BillboardCalc>();
	//systemManager_.AddSystem<ECS::System::BoneAnimationCalc>(&boneModel_);
	systemManager_.AddSystem<ECS::System::SlideFollowCameraUpdate>();
	//systemManager_.AddSystem<ECS::System::BoneDrawer>(&boneModel_);
	systemManager_.AddSystem<ECS::System::CalcTransMatrix>();
	systemManager_.AddSystem<ECS::System::CalcParentTransform>();
	systemManager_.AddSystem<ECS::System::ModelDrawer>();
	systemManager_.AddSystem<ECS::System::DrawHelthBar>(healthBar_.get());
	systemManager_.AddSystem<ECS::System::DrawEnemyHelthBar>(&enemyHealthBar_);
	systemManager_.AddSystem<ECS::System::CursorDrawer>();

	offScreen_ = PostEffect::OffScreenRenderer::GetInstance();
	offScreen_->Init();

	fullScreen_ = PostEffect::FullScreenRenderer::GetInstance();
	fullScreen_->Init({ { L"FullScreen.VS.hlsl",L"FullScreen.PS.hlsl" }, { L"FullScreen.VS.hlsl",L"Vignetting.PS.hlsl" } , { L"FullScreen.VS.hlsl",L"Vignetting.PS.hlsl" } });

	ModelManager::GetInstance()->AddModel("Box", Model::LoadAssimpModelFile("Model/AnimatedCube/", "AnimatedCube.gltf"));

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "test.json" });

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, world_.get());


}

void GameScene::OnExit() {
	audio_->StopAllWave();
	offScreen_->Finalize();
	fullScreen_->Finalize();
}

void GameScene::Update() {

	static bool skeletonDraw = true;
	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	[[maybe_unused]] const float powDeltaTime = deltaTime * deltaTime;

	ImGui::Text("XInput左スティックで移動");
	ImGui::Text("エンティティ数 / %lu", world_->size());
	if (ImGui::Button("SkeletonDebug")) {
		skeletonDraw = not skeletonDraw;
	}

	light_->ImGuiWidget();
	gameObject_.GetComponent<ModelComp>()->GetBone("Body")->ImGuiWidget();

	isGrayScale_--;

	particleArray_.clear();

	blockRender_->clear();
	skinModelRender_->clear();
	spawnTimer_.Update(deltaTime);
	playerSpawn_.Update(deltaTime);

	// エンティティの追加
	spawner_.ActivateSpawn(entityManager_);
	// プレハブの破棄
	spawner_.clear();


	constexpr uint32_t enemyCount = 5u;

	if (spawnTimer_.IsFinish()) {
		// スポナーに追加を要求する
		spawner_.AddSpawner(enemyPrefab_.get(), enemyCount, [](auto enemys, auto manager)
			{
				for (uint32_t i = 0; auto & enemy : enemys) {
					const auto &[pos] = manager->GetComponent<ECS::PositionComp>(enemy);
					pos->position_ = SoLib::EulerToDirection(SoLib::Euler{ 0.f, (Angle::PI2 / enemyCount) * i, 0.f }) * 7.5f + pos->position_;
					i++;
				}
			});
		spawnTimer_.Start();
	}
	// プレイヤのView
	auto playerView = world_->view<const ECS::PlayerTag>();
	// プレイヤのViewの長さが0である場合は死んでいる
	bool playerIsDead = playerView.begin() == playerView.end();

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
	world_->erase_if(std::function<bool(ECS::Entity *, ECS::IsAlive *)>(
		[](ECS::Entity *, ECS::IsAlive *a)
		{
			return not a->isAlive_;
		}
	));
	// ここでECSのsystemを呼び出す
	systemManager_.Update(world_.get(), deltaTime);

	if (skeletonDraw) {
		for (const auto &[entity, skinModel, mat] : world_->view<const ECS::SkinModel, const ECS::TransformMatComp>()) {

			skinModel->skinModel_->skeleton_->AddDrawBuffer(*mat, { .z = -2.f });
		}
	}

	ground_.Draw();

	cameraManager_->Update(deltaTime);

	gameObject_.Update(deltaTime);


	ImGui::DragFloat2("VignettingParam", &fullScreen_->GetFParam()->first);

	float health = 0.f;


	for (const auto &[entity, healthComp, player] : world_->view<const ECS::HealthComp, const ECS::PlayerTag>()) {

		health = healthComp->CalcPercent();
	}
	fullScreen_->GetFParam()->first = SoLib::Lerp(0.5f, 16.f, health);

	for (const auto &[entity, color, billboard, mat] : world_->view<const ECS::Color, const ECS::BillboardRotate, const ECS::TransformMatComp>()) {

		particleArray_.push_back(Particle::ParticleData{ .transform = mat->transformMat_, .color = color->color_ });

	}

	particleManager_->Update(deltaTime);
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


	//const auto &camera = *cameraManager_->GetUseCamera();

#pragma region モデル描画

	Model::StartDraw(commandList);

	//light_->SetLight(commandList);

	//gameObject_.Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);

	blockRender_->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kSkinParticle);
	light_->SetLight(commandList);

	skinModelRender_->Draw(camera);

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
		if (count >= systemManager_.GetSystem<ECS::System::DrawEnemyHelthBar>()->drawCount_) { break; }

		bar->Draw();
		count++;
	}

	healthBar_->Draw();
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

	pDxCommon_->DefaultDrawReset(false);

	fullScreen_->Draw({ L"FullScreen.VS.hlsl",L"Vignetting.PS.hlsl" }, offScreen_->GetTexture(), offScreen_->GetHeapRange()->GetHandle(0).gpuHandle_);


}
