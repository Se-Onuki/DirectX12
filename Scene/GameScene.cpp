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
	archetype.AddClassData<ECS::Identifier, ECS::ModelComp>();

	mArray_ = std::make_unique<ECS::MultiArray>(archetype);

	size_t entityID = mArray_->push_back();
	mArray_->GetItem<ECS::Identifier>(entityID).name_ = "hello";
	mArray_->GetItem<ECS::Identifier>(mArray_->push_back()).name_ = "goodbye";
	mArray_->GetItem<ECS::Identifier>(mArray_->push_back()).name_ = "hi";

	mArray_->swap(0, 2);


}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	// const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	light_->ImGuiWidget();
	//ECS::Viewer<ECS::Identifier> view{  mArray_->GetItem<ECS::Identifier>(0u)  };

	auto [item] = static_cast<std::tuple<ECS::Identifier *>>(mArray_->GetChunk()[0]->get<ECS::Identifier>().begin());

	ImGui::Text("%s", mArray_->GetItem<ECS::Identifier>(0).name_.data());
	ImGui::Text("%s", mArray_->GetItem<ECS::Identifier>(1).name_.data());
	ImGui::Text("%s", mArray_->GetItem<ECS::Identifier>(2).name_.data());

	ImGui::Text("viewTest %s", item->name_.data());

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