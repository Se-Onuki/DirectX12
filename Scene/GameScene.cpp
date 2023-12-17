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
	archetype.AddClassData<ECS::Identifier, ECS::ModelComp, ECS::IsAlive>();

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
		const auto &[name, model, alive] = mArray_->create_back<ECS::Identifier, ECS::ModelComp, ECS::IsAlive>();
		// データを代入
		name->name_ = std::string("test") + std::to_string(i + 10u);
		if (i % 2u == 0u) {
			alive->isAlive_ = false;
		}
	}

	mArray_->erase_if(std::function<bool(const ECS::IsAlive *const)>([](const ECS::IsAlive *const a) { if (a->isAlive_ == false) { return true; } return false; }));

}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	// const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	light_->ImGuiWidget();

	for (const auto &[id, model] : mArray_->get<ECS::Identifier, ECS::ModelComp>()) {
		ImGui::Text("%s,%x", id->name_.data(), model->model_);
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