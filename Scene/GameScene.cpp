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
	cameraManager_->Init();

	[[maybe_unused]] bool a = file_.Load("resources/Level/Level1.csv");
	csv_ = file_;

	light_ = DirectionLight::Create();

	ModelManager::GetInstance()->AddModel("Block", Model::LoadObjFile("", "box.obj"));
	mapChip_ = std::make_unique<MapChip>();
	mapChip_->Init();
	mapChip_->Load(csv_);

	pBlockManager_ = BlockManager::GetInstance();
	pBlockManager_->Init(1024u);

}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	std::stringstream outPut{};
	outPut << csv_;
	ImGui::Text("%s", outPut.str().c_str());

	cameraManager_->Update(deltaTime);
	cameraManager_->DisplayImGui();

	//light_->ImGuiWidget();

	mapChip_->Update(deltaTime);

}

void GameScene::Draw() {

	pBlockManager_->clear();
	mapChip_->Draw();

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

	pBlockManager_->Draw(camera);


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}