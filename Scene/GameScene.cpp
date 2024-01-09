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
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {

	light_ = DirectionLight::Create();


	world_ = std::make_unique<World>();
	entityManager_ = world_->GetEntityManager();
	cameraManager_->Init();
	ModelManager::GetInstance()->CreateDefaultModel();

	ModelManager::GetInstance()->AddModel("Block", Model::LoadObjFile("", "box.obj"));
	model_ = ModelManager::GetInstance()->GetModel("Plane");
	model_->materialMap_.begin()->second->texHandle_ = TextureManager::Load("circle.png");

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
	*prefab_ += ECS::TransformMatComp{};
	*prefab_ += ECS::AliveTime{};
	*prefab_ += ECS::EmitterComp{ .count_ = 5u,.startColor_ = 0xFFFF00FF,.endColor_ = 0xFF000000,.spawnLifeLimit_{0.1f,0.2f},.spawnPower_{0.05f,0.1f},.spawnRange_{90._deg,180._deg,0.f} };
	*prefab_ += ECS::InputFlagComp{};

	entityManager_->CreateEntity(*prefab_);

	enemyPrefab_ = std::make_unique<ECS::Prefab>();

	*enemyPrefab_ += ECS::IsAlive{};
	*enemyPrefab_ += ECS::PositionComp{};
	*enemyPrefab_ += ECS::PositionComp{};
	*enemyPrefab_ += ECS::QuaternionRotComp{};
	//*enemyPrefab_ += ECS::RotateComp{};




	/*for (const auto &emitter : emitterList) {
		const auto &[lifeLimit] = entityManager_->GetComponent<ECS::LifeLimit>(emitter);
		lifeLimit->lifeLimit_ = -1.f;
	}*/

	//// エンティティの追加(idは先頭からのindex)
	//size_t entityID = mArray_->push_back();
	//// 添え字を使って要素にアクセス。
	//mArray_->GetItem<ECS::Identifier>(entityID).name_ = "hello";
	//mArray_->GetItem<ECS::Identifier>(mArray_->push_back()).name_ = "goodbye";
	//mArray_->GetItem<ECS::Identifier>(mArray_->push_back()).name_ = "hi";

	//for (uint32_t i = 0u; i < 10u; i++) {
	//	size_t index = mArray_->push_back();
	//	// 要素を追加し、その要素のデータをイテレータから取得してstd::tupleで展開する
	//	const auto &[name, model] = (*mArray_->get<ECS::Identifier, ECS::ModelComp>().begin()[index]);
	//	// データを代入
	//	name->name_ = std::string("test") + std::to_string(i);

	//	model->model_ = model_;
	//}

	//for (uint32_t i = 0u; i < 10u; i++) {
	//	// 作成されたデータへのポインタ
	//	const auto &[name, model, aliveTime] = mArray_->create_back<ECS::Identifier, ECS::ModelComp, ECS::LifeLimit>();
	//	// データを代入
	//	name->name_ = std::string("test") + std::to_string(i);
	//	//if (i % 2u == 0u) {
	//	aliveTime->lifeLimit_ = 2.f;
	//	//}
	//	model->model_ = model_;
	//
	//}

	/*const auto &[id, emittComp] = emitterArray_->create_back<ECS::Identifier, ECS::EmitterComp>();
	id->name_ = "emitter";
	emittComp->count_ = 1u;
	emittComp->frequency_.Start(0.2f);*/



}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	ImGui::Text("ParticleCount / %lu", world_->Count());

	static SoLib::Color::RGB4 testColor;

	ImGui::ColorEdit4("TestColor", testColor, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_PickerHueWheel);

	cameraManager_->DisplayImGui();
	cameraManager_->Update(deltaTime);

	particleArray_.clear();

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


	for (const auto &[aliveTime, lifelimit, colorLerp, color] : world_->view<ECS::AliveTime, ECS::LifeLimit, ECS::ColorLarp, ECS::Color>()) {
		color->color_ = colorLerp->EaseColor(aliveTime->aliveTime_ / lifelimit->lifeLimit_);
	}

	for (const auto &[pos, velocity] : world_->view<ECS::PositionComp, ECS::VelocityComp>()) {

		pos->position_ += *velocity;

	}

	for (const auto &[pos, input] : world_->view<ECS::PositionComp, ECS::InputFlagComp>()) {
		Vector2 inputLs = input_->GetXInput()->GetState()->stickL_;
		pos->position_ += Vector3{ inputLs.x,0.f,inputLs.y }*0.1f;

	}

	for (const auto &[pos, rot, scale, transMat, emitterComp] : world_->view<ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::EmitterComp>()) {

		*transMat = Matrix4x4::Affine(*scale, rot->rotate_, *pos);

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

				Vector3 direction;
				direction.x = cos(fireDir.x) * cos(fireDir.y);
				direction.y = sin(fireDir.x) * cos(fireDir.y);
				direction.z = -sin(fireDir.y);

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