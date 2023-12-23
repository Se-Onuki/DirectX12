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

	world_ = World::GetInstance();
	// world_->GetEntityManager()->CreateEntity<ECS::TransformComp>();



	Archetype archetype;
	archetype.AddClassData<ECS::Identifier, ECS::ModelComp, ECS::IsAlive, ECS::PositionComp, ECS::RotateComp, ECS::ScaleComp, ECS::TransformMatComp, ECS::AliveTime, ECS::Color>();

	mArray_ = std::make_unique<ECS::MultiArray>(archetype);

	// エンティティの追加(idは先頭からのindex)
	size_t entityID = mArray_->push_back();
	// 添え字を使って要素にアクセス。
	mArray_->GetItem<ECS::Identifier>(entityID).name_ = "hello";
	mArray_->GetItem<ECS::Identifier>(mArray_->push_back()).name_ = "goodbye";
	mArray_->GetItem<ECS::Identifier>(mArray_->push_back()).name_ = "hi";

	for (uint32_t i = 0u; i < 10u; i++) {
		size_t index = mArray_->push_back();
		// 要素を追加し、その要素のデータをイテレータから取得してstd::tupleで展開する
		const auto &[name, model] = (*mArray_->get<ECS::Identifier, ECS::ModelComp>().begin()[index]);
		// データを代入
		name->name_ = std::string("test") + std::to_string(i);
	}

	for (uint32_t i = 0u; i < 10u; i++) {
		// 作成されたデータへのポインタ
		const auto &[name, model, aliveTime] = mArray_->create_back<ECS::Identifier, ECS::ModelComp, ECS::AliveTime>();
		// データを代入
		name->name_ = std::string("test") + std::to_string(i + 10u);
		if (i % 2u == 0u) {
			aliveTime->lifeLimit_ = i * 1.5f;
		}
	}
}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	SoLib::ImGuiWidget("ColorFrom", &rgbFrom_);
	SoLib::ImGuiWidget("ColorTo", &rgbTo_);
	SoLib::ImGuiWidget("ColorT", &t_);
	SoLib::Color::RGB4 resultColor = SoLib::Lerp(rgbFrom_, rgbTo_, t_);
	SoLib::ImGuiWidget("ResultColor", &resultColor);

	//light_->ImGuiWidget();

	// もし生存フラグが折れていたら、配列から削除
	mArray_->erase_if(std::function<bool(ECS::IsAlive *)>(
		[](ECS::IsAlive *a)
		{
			return not a->isAlive_;
		}
	));

	for (const auto &[aliveTime, isAlive] : mArray_->get<ECS::AliveTime, ECS::IsAlive>()) {
		// もし寿命が定められていたら
		if (aliveTime->lifeLimit_ != 0.f) {
			// 寿命を超過していたら
			if (aliveTime->lifeLimit_ < aliveTime->aliveTime_) {
				// 死ぬ
				isAlive->isAlive_ = false;
			}
		}
	}

	for (const auto &[aliveTime] : mArray_->get<ECS::AliveTime>()) {
		aliveTime->aliveTime_ += deltaTime;
	}

	for (const auto &[id, aliveTime] : mArray_->get<ECS::Identifier, ECS::AliveTime>()) {
		ImGui::Text("%s : %f", id->name_.data(), aliveTime->aliveTime_);
	}


	for (const auto &[id, model, pos, rot] : mArray_->get<ECS::Identifier, ECS::ModelComp, ECS::PositionComp, ECS::RotateComp>()) {
		ImGui::Text("%s,%x\n", id->name_.data(), model->model_);
		SoLib::ImGuiWidget((id->name_.data() + std::string(" : pos")).c_str(), &pos->position_);
		SoLib::ImGuiWidget((id->name_.data() + std::string(" : rot")).c_str(), &rot->rotate_);
	}

	if (input_->GetDirectInput()->IsTrigger(DIK_P)) {
		auto mSubArray = mArray_->get<ECS::IsAlive>();
		auto mArrBegin = mSubArray.begin();
		if (mArrBegin != mSubArray.end()) {

			const auto &[alive] = *mArrBegin;
			alive->isAlive_ = false;
		}
	}

	auto mSubArray = mArray_->get<ECS::Identifier>();
	auto mArrBegin = mSubArray.begin();
	if (mArrBegin != mSubArray.end()) {

		const auto &[name] = *mArrBegin;

		SoLib::ImGuiWidget("Name", &name->name_);

	}

}

void GameScene::Draw() {
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

	light_->SetLight(commandList);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}