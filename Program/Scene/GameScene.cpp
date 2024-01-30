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

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cameraManager_ = CameraManager::GetInstance();
	blockRender_ = BlockManager::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {

	light_ = DirectionLight::Create();

	blockRender_->Init(1024u);


	world_ = std::make_unique<World>();
	entityManager_ = world_->GetEntityManager();
	ModelManager::GetInstance()->CreateDefaultModel();

	boxModel_ = ModelManager::GetInstance()->AddModel("Block", Model::LoadObjFile("", "box.obj"));
	model_ = ModelManager::GetInstance()->AddModel("Particle", Model::CreatePlane());
	model_->materialMap_.begin()->second->texHandle_ = TextureManager::Load("circle.png");

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

	Archetype particleArchetype;
	particleArchetype.AddClassData<ECS::Identifier, ECS::ModelComp, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::BillboardRotate, ECS::Color, ECS::VelocityComp, ECS::ColorLarp>();

	entityManager_->CreateEntity(particleArchetype, 3u);
	//auto emitterList = entityManager_->CreateEntity<ECS::Identifier, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::EmitterComp>();

	prefab_ = std::make_unique<ECS::Prefab>();

	*prefab_ += ECS::Identifier{};
	*prefab_ += ECS::IsAlive{};
	*prefab_ += ECS::PositionComp{};
	*prefab_ += ECS::RotateComp{};
	*prefab_ += ECS::ScaleComp{};
	*prefab_ += ECS::AliveTime{};
	*prefab_ += ECS::EmitterComp{ .count_ = 5u,.startColor_ = 0xFFFF00FF,.endColor_ = 0xFF000000,.spawnLifeLimit_{0.1f,0.2f},.spawnPower_{0.05f,0.1f},.spawnRange_{90._deg,180._deg,0.f} };

	entityManager_->CreateEntity(*prefab_);

	playerPrefab_ = std::make_unique<ECS::Prefab>();

	*playerPrefab_ += ECS::IsAlive{};
	*playerPrefab_ += ECS::ScaleComp{};
	*playerPrefab_ += ECS::QuaternionRotComp{};
	*playerPrefab_ += ECS::PositionComp{};
	// *enemyPrefab_ += ECS::TransformMatComp{};
	*playerPrefab_ += ECS::InputFlagComp{};
	*playerPrefab_ += ECS::BoneTransformComp{ .boneTransform_{{BoneModel::SimpleTransform{},BoneModel::SimpleTransform{.translate_{0.f,1.f,0.f}}}} };
	*playerPrefab_ += ECS::VelocityComp{};
	*playerPrefab_ += ECS::AccelerationComp{};
	*playerPrefab_ += ECS::GravityComp{ .gravity_{.y = -9.8f} };
	*playerPrefab_ += ECS::CollisionComp{ .collision_ = Sphere{.centor {.y = 1.f}, .radius = 1.f} };
	*playerPrefab_ += ECS::PlayerTag{};
	*playerPrefab_ += ECS::WeaponComp{ .collision_{.radius = 1.f} };
	*playerPrefab_ += ECS::AnimateParametor{};

	enemyPrefab_ = std::make_unique<ECS::Prefab>();
	*enemyPrefab_ += ECS::IsAlive{};
	*enemyPrefab_ += ECS::ScaleComp{};
	*enemyPrefab_ += ECS::QuaternionRotComp{};
	*enemyPrefab_ += ECS::PositionComp{ .position_{0.f, 1.f, 10.f} };
	*enemyPrefab_ += ECS::TransformMatComp{};
	*enemyPrefab_ += ECS::ModelComp{ .model_{ModelManager::GetInstance()->GetModel("Block")} };
	// *enemyPrefab_ += ECS::BoneTransformComp{ .boneTransform_{{BoneModel::SimpleTransform{},BoneModel::SimpleTransform{.translate_{0.f,1.f,0.f}}}} };
	*enemyPrefab_ += ECS::GravityComp{ .gravity_{.y = -9.8f} };
	*enemyPrefab_ += ECS::CollisionComp{ .collision_ = Sphere{.radius = 1.f} };
	*enemyPrefab_ += ECS::EnemyTag{};

	entityManager_->CreateEntity(*enemyPrefab_);

	/*Archetype enemyArchetype{};
	enemyArchetype.AddClassData<ECS::IsAlive, ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::TransformMatComp, ECS::InputFlagComp, ECS::BoneTransformComp>();*/

	entityManager_->CreateEntity(*playerPrefab_);

	Archetype followCameraArchetype{};
	followCameraArchetype.AddClassData<ECS::FollowCamera, ECS::PositionComp>();

	entityManager_->CreateEntity(followCameraArchetype);

	//*enemyPrefab_ += ECS::RotateComp{};

	soundA_ = audio_->LoadWave("resources/Alarm01.wav");

	boneModel_.Init();
	auto *bodyPtr = boneModel_.AddBone("Body", boxModel_);
	boneModel_.AddBone("Head", boxModel_, bodyPtr);
	boneModel_.AddBone("Sword", boxModel_, bodyPtr);

	boneModel_.SetNumber();

	ground_.Init();

	spawnTimer_.Start();

}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	[[maybe_unused]] const float powDeltaTime = deltaTime * deltaTime;

	ImGui::Text("XInput左スティックで移動");
	ImGui::Text("エンティティ数 / %lu", world_->size());

	static SoLib::Color::HSV4 testColor;

	ImGui::ColorEdit4("TestColor", testColor, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_PickerHueWheel);
	SoLib::ImGuiWidget("SoLibColor", &testColor);

	const SoLib::Math::Euler eulerRotate{ -30._deg,0._deg,0._deg };


	ImGui::Text("%s", SoLib::to_string(Vector3::front * Matrix4x4::EulerRotate(eulerRotate)).c_str());
	ImGui::Text("%s", SoLib::to_string(Quaternion::RotateVector(Vector3::front, SoLib::MakeQuaternion(eulerRotate))).c_str());
	ImGui::Text("%s", SoLib::to_string(Vector3::front * SoLib::MakeQuaternion(eulerRotate).MakeRotateMatrix()).c_str());
	ImGui::Text("%s", SoLib::to_string(SoLib::EulerToDirection(eulerRotate)).c_str());
	ImGui::Text("%s", SoLib::to_string(Quaternion::LookAt(SoLib::EulerToDirection(eulerRotate)).RotateVector(Vector3::front)).c_str());

	particleArray_.clear();

	blockRender_->clear();
	spawnTimer_.Update(deltaTime);

	if (spawnTimer_.IsFinish()) {

		entityManager_->CreateEntity(*enemyPrefab_);
		spawnTimer_.Start();
	}

	// もし生存フラグが折れていたら、配列から削除
	world_->erase_if(std::function<bool(ECS::IsAlive *)>(
		[](ECS::IsAlive *a)
		{
			return not a->isAlive_;
		}
	));

	for (const auto &[aliveTime, lifeLimit, isAlive] : world_->view<ECS::AliveTime, ECS::LifeLimit, ECS::IsAlive>()) {
		// もし寿命が定められていたら
		if (lifeLimit->lifeLimit_ >= 0.f) {
			// 寿命を超過していたら
			if (lifeLimit->lifeLimit_ < aliveTime->aliveTime_) {
				// 死ぬ
				isAlive->isAlive_ = false;
			}
		}
	}

	for (const auto &[aliveTime] : world_->view<ECS::AliveTime>()) {
		// 生存時間を加算
		aliveTime->aliveTime_ += deltaTime;
	}

	for (const auto &[animate] : world_->view<ECS::AnimateParametor>()) {
		auto &timer = animate->timer_;

		// 動作パラメータを追加
		timer.Update(deltaTime);

		if (timer.IsActive() && timer.IsFinish()) {
			// 次のアニメーションの情報を格納

			// 仮実装なので、マジックナンバーで行う
			if (animate->animIndex_ == 0u) {
				animate->animIndex_ = 1u;
				timer.Start();
			}
			else {
				animate->animIndex_ = 0u;
			}


		}
	}


	for (const auto &[aliveTime, lifelimit, colorLerp, color] : world_->view<ECS::AliveTime, ECS::LifeLimit, ECS::ColorLarp, ECS::Color>()) {
		color->color_ = colorLerp->EaseColor(aliveTime->aliveTime_ / lifelimit->lifeLimit_);
	}

	for (const auto &[acceleration, gravity] : world_->view<ECS::AccelerationComp, ECS::GravityComp>()) {

		acceleration->acceleration_ += gravity->gravity_ * deltaTime;

	}
	for (const auto &[velocity, acceleration] : world_->view<ECS::VelocityComp, ECS::AccelerationComp>()) {

		velocity->velocity_ += acceleration->acceleration_;
		acceleration->acceleration_ = {};

	}

	for (const auto &[pos, velocity] : world_->view<ECS::PositionComp, ECS::VelocityComp>()) {

		pos->position_ += velocity->velocity_ * deltaTime;

	}

	{
		Vector3 playerPos{};
		for (const auto &[player, plPos] : world_->view<ECS::PlayerTag, ECS::PositionComp>()) {
			playerPos = *plPos;
		}

		for (const auto &[enemy, pos, rotate] : world_->view<ECS::EnemyTag, ECS::PositionComp, ECS::QuaternionRotComp>()) {
			Vector3 direction = (playerPos - pos->position_).Nomalize() * 100.f * powDeltaTime;
			direction.y = 0.f;
			pos->position_ += direction;

			rotate->quateRot_ = Quaternion::LookAt(direction);

		}
	}

	for (const auto &[collision, pos, velocity] : world_->view<ECS::CollisionComp, ECS::PositionComp, ECS::VelocityComp>()) {
		// 地面より座標が下なら
		if ((pos->position_.y + collision->collision_.centor.y - collision->collision_.radius) < ground_.hight_) {
			// 地面の上に当たり判定を上にする
			pos->position_.y = ground_.hight_ - collision->collision_.centor.y + collision->collision_.radius;
			// もし落下していたら
			if (velocity->velocity_.y < 0.f) {
				// 移動速度を0にする
				velocity->velocity_.y = 0.f;
			}
		}
	}

	{
		std::list<ECS::WeaponComp *> weaponList{};

		for (const auto &[weapon] : world_->view<ECS::WeaponComp>()) {
			weaponList.push_back(weapon);
		}

		for (const auto &[enemy, pos, collision, isAlive] : world_->view<ECS::EnemyTag, ECS::PositionComp, ECS::CollisionComp, ECS::IsAlive>()) {

			Sphere sphere = collision->collision_;
			sphere.centor += *pos;

			for (const auto *weapon : weaponList) {
				if (Collision::IsHit(sphere, weapon->collision_)) {
					isAlive->isAlive_ = false;
				}
			}

		}
	}

	for (const auto &[pos, quateRot, acceleration, input, animate] : world_->view<ECS::PositionComp, ECS::QuaternionRotComp, ECS::AccelerationComp, ECS::InputFlagComp, ECS::AnimateParametor>()) {
		const auto *const camera = cameraManager_->GetUseCamera();
		const Vector2 inputLs = input_->GetXInput()->GetState()->stickL_;
		const Vector3 input3d{ inputLs.x,0.f,inputLs.y };
		const Vector3 rotateInput = Quaternion::AnyAxisRotation(Vector3::up, camera->rotation_.y).RotateVector(input3d);
		pos->position_ += rotateInput * (500.f * powDeltaTime);

		if (input3d.LengthSQ()) {
			quateRot->quateRot_ = Quaternion::LookAt(rotateInput);
		}

		if (input_->GetXInput()->IsTrigger(KeyCode::A)) {
			acceleration->acceleration_.y += 10.f;
		}

		if (animate->animIndex_ == 0u && animate->timer_.IsFinish()) {
			if (input_->GetXInput()->GetState()->triggerR_ > 0.5f) {
				animate->timer_.Start(0.5f);
			}
		}
	}



	for (const auto &[pos, rot, scale, emitterComp] : world_->view<ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::EmitterComp>()) {

		//*transMat = Matrix4x4::Affine(*scale, rot->rotate_, *pos);

		SoLib::ImGuiWidget("Emitter : Pos", &pos->position_);
		SoLib::ImGuiWidget("Emitter", emitterComp);

		emitterComp->frequency_.Update(deltaTime);
		if (emitterComp->frequency_.IsFinish()) {
			emitterComp->frequency_.Start();
			auto particleList = entityManager_->CreateEntity<ECS::Identifier, ECS::ModelComp, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::LifeLimit, ECS::BillboardRotate, ECS::Color, ECS::VelocityComp, ECS::ColorLarp>(emitterComp->count_);

			for (const auto &particle : particleList) {
				const auto &[cId, cModel, cLifeLimit, cPos, cVelocity, colorLerp] = entityManager_->GetComponent<ECS::Identifier, ECS::ModelComp, ECS::LifeLimit, ECS::PositionComp, ECS::VelocityComp, ECS::ColorLarp>(particle);

				cModel->model_ = model_;
				cLifeLimit->lifeLimit_ = emitterComp->spawnLifeLimit_.Random();

				SoLib::Math::Euler fireDir = emitterComp->spawnRange_;
				for (auto &item : fireDir) {
					item = Random::GetRandom<float>(-item, item);
				}

				Vector3 direction = SoLib::Math::EulerToDirection(fireDir);

				*cVelocity = direction * emitterComp->spawnPower_.Random()/* * Matrix4x4::EulerRotate(fireDir)*/;

				*cPos = *pos;

				colorLerp->start_ = emitterComp->startColor_;
				colorLerp->end_ = emitterComp->endColor_;

			}
		}
	}

	Matrix4x4 billboardMat = cameraManager_->GetUseCamera()->matView_.GetRotate().InverseRT();

	for (const auto &[scale, rotate, pos, mat, billboardRot] : world_->view<ECS::ScaleComp, ECS::RotateComp, ECS::PositionComp, ECS::TransformMatComp, ECS::BillboardRotate>()) {

		*mat = Matrix4x4::Affine(*scale, rotate->rotate_, Vector3::zero);
		mat->transformMat_ *= billboardMat;
		*reinterpret_cast<Vector3 *>(mat->transformMat_.m[3]) = *pos;

	}

	for (const auto &[scale, quate, pos, bone, animate] : world_->view<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::BoneTransformComp, ECS::AnimateParametor>()) {

		bone->boneTransform_[0] = { *scale, quate->quateRot_.Normalize(), *pos };

		// 頭のパラメータ
		{
			auto &head = bone->boneTransform_[boneModel_.GetIndex("Head")];
			head.translate_.y = 2.f;
			head.scale_ = Vector3::one * 0.5f;
		}

		// 体のパラメータ
		{
			auto &body = bone->boneTransform_[boneModel_.GetIndex("Body", 0)];

			body.scale_ = { 0.75f,1.f,0.75f };
		}

		// 剣のパラメータ
		{
			auto &swordModel = bone->boneTransform_[boneModel_.GetIndex("Sword", 0)];

			swordModel.translate_.y = 3.f;

			swordModel.scale_ = { 0.25f,1.f,0.25f };


			auto &sword = bone->boneTransform_[boneModel_.GetIndex("Sword")];

			if (not animate->timer_.IsFinish()) {
				if (animate->animIndex_ == 0u) {
					sword.rotate_ = Quaternion::AnyAxisRotation(Vector3::right, 90._deg * SoLib::easeInOutSine(animate->timer_.GetProgress()));
				}
				else {
					sword.rotate_ = Quaternion::AnyAxisRotation(Vector3::right, 90._deg * SoLib::easeInOutSine(1.f - animate->timer_.GetProgress()));
				}
			}
		}


	}



	for (const auto &[bone, weapon] : world_->view<ECS::BoneTransformComp, ECS::WeaponComp>()) {
		auto matrixArray = boneModel_.CalcTransMat(bone->boneTransform_);

		boneModel_.Draw(matrixArray);

		weapon->collision_.centor = *reinterpret_cast<Vector3 *>(matrixArray[boneModel_.GetIndex("Sword", 0)].m[3]);
	}

	for (const auto &[scale, quate, pos, mat] : world_->view<ECS::ScaleComp, ECS::QuaternionRotComp, ECS::PositionComp, ECS::TransformMatComp>()) {

		mat->transformMat_ = SoLib::Math::Affine(*scale, quate->quateRot_.Normalize(), *pos);

		//blockRender_->AddBox(boxModel_, IBlock{ .transMat_ = bone->boneTransform_[0].CalcTransMat() });

	}

	for (const auto &[model, translateMat, enemy] : world_->view<ECS::ModelComp, ECS::TransformMatComp, ECS::EnemyTag>()) {
		blockRender_->AddBox(model->model_, IBlock{ .transMat_ = translateMat->transformMat_ });

	}

	/*static uint32_t index = 0u;
	SoLib::ImGuiWidget<BoneModel::SimpleTransform, decltype(boneTransform_)>("BoneTransform", &boneTransform_, index);

	boneModel_.Draw(boneModel_.CalcTransMat(boneTransform_));*/


	blockRender_->AddBox(ground_.model_, IBlock{ .transMat_ = ground_.CalcMatrix() });

	for (const auto &[player, pos] : world_->view<ECS::PlayerTag, ECS::PositionComp>()) {


		for (const auto &[followCamera, cameraPos] : world_->view<ECS::FollowCamera, ECS::PositionComp>()) {
			*cameraPos = pos->position_;

			followCamera->rotation_.y += input_->GetXInput()->GetState()->stickR_.x * 90._deg * deltaTime;

			followCamera->TransferData(*cameraManager_->GetUseCamera(), *cameraPos);
		}
	}

	cameraManager_->DisplayImGui();
	cameraManager_->Update(deltaTime);


	for (const auto &[color, billboard, mat] : world_->view<ECS::Color, ECS::BillboardRotate, ECS::TransformMatComp>()) {

		particleArray_.push_back(Particle::ParticleData{ .transform = mat->transformMat_ ,.color = color->color_ });

	}
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

	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);

	blockRender_->Draw(camera);

	Model::SetPipelineType(Model::PipelineType::kParticle);

	light_->SetLight(commandList);

	model_->Draw(particleArray_, camera);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}