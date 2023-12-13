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

	ModelManager::GetInstance()->CreateDefaultModel();
	ModelManager::GetInstance()->AddModel("Box", Model::LoadObjFile("", "box.obj"));
	model_ = ModelManager::GetInstance()->GetModel("Plane");

	world_ = World::GetInstance();
	world_->GetEntityManager()->CreateEntity<ECS::Component::TransformComp>();

	Archetype archetype;
	archetype.AddClassData<ECS::Component::Identifier, ECS::Component::ModelComp>();

	mArray_ = std::make_unique<ECS::MultiArray>(archetype);

	size_t entityID = mArray_->push_back();
	mArray_->GetItem<ECS::Component::Identifier>(entityID).name_ = "hello";
	mArray_->GetItem<ECS::Component::Identifier>(mArray_->push_back()).name_ = "goodbye";

	particles_ = std::make_unique<ArrayBuffer<Particle::ParticleData>>(256u);
	// デバイスの取得
	auto *device = DirectXCommon::GetInstance()->GetDevice();
	// SRVヒープの取得
	auto *srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

	heapRange_ = srvHeap->RequestHeapAllocation(1u);
	device->CreateShaderResourceView(particles_->GetResources(), &particles_->GetDesc(), heapRange_.GetHandle(0u).cpuHandle_);

	indexBegin_ = 0u;

	registry_ = std::make_unique<entt::registry>();

	for (auto i = 0u; i < 256u; ++i) {
		const auto entity = registry_->create();
		registry_->emplace<ECS::Component::Position>(entity);
		registry_->emplace<ECS::Component::Velocity>(entity, ECS::Component::Velocity{ .velocity_ = Vector3{ Random::GetRandom(-1.f, 1.f)  ,Random::GetRandom(-1.f, 1.f),Random::GetRandom(-1.f, 1.f) }*0.01f });
		registry_->emplace<ECS::Component::Color>(entity, ECS::Component::Color{ .color_ = Vector4::one });
		registry_->emplace<ECS::Component::TransformMatrix>(entity);
	}
}

void GameScene::OnExit() {
	audio_->StopAllWave();
}

void GameScene::Update() {

	// const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	light_->ImGuiWidget();

	ImGui::Text("%s", mArray_->GetItem<ECS::Component::Identifier>(0).name_.data());
	ImGui::Text("%s", mArray_->GetItem<ECS::Component::Identifier>(1).name_.data());

	camera_.ImGuiWidget();

	camera_.UpdateMatrix();


	model_->ImGuiWidget();

	EnttUpdate(*registry_);
	EnttDraw(particles_.get(), particleIndex_, *registry_);

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

	Model::SetPipelineType(Model::PipelineType::kParticle);

	light_->SetLight(commandList);

	model_->Draw(heapRange_.GetHandle(0u).gpuHandle_, particleIndex_, indexBegin_, camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}