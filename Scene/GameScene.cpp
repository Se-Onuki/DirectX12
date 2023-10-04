#include "GameScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"

#include "TitleScene.h"

#include "../Header/Entity/Component/ModelComp.h"
#include "../Utils/SoLib/SoLib_ImGui.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandIe.h"

GameScene::GameScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// collisionManager_ = CollisionManager::GetInstance();
}

GameScene::~GameScene() {

}

void GameScene::OnEnter() {
	light_.reset(DirectionLight::Create());

	ModelManager::GetInstance()->CreateDefaultModel();

	model_ = ModelManager::GetInstance()->GetModel("Plane");
	transform_.UpdateMatrix();
	camera_.Init();

	sprite_.reset(Sprite::Create(TextureManager::Load("white2x2.png")));
	sprite_->SetScale({ 100.f,100.f });

	auto *const device = DirectXCommon::GetInstance()->GetDevice();
	auto *const srvHeap = TextureManager::GetInstance()->GetSRVHeap();
	const auto srvSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	const uint32_t instancingCount = 5u;
	instancingData_ = CreateBufferResource(device, sizeof(Transform::TransformMatrix) * instancingCount);

	instancingData_->Map(0, nullptr, reinterpret_cast<void **>(&instancingArray_));

	for (uint32_t i = 0; i < instancingCount; ++i) {
		instancingArray_[i].World = Matrix4x4::Identity();
	}


	srvDesc_.Format = DXGI_FORMAT_UNKNOWN;	// 構造体の形は不明であるため
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	// textureではなくbufferとして使うため
	srvDesc_.Buffer.FirstElement = 0;
	srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc_.Buffer.StructureByteStride = sizeof(Transform::TransformMatrix);	// アライメントはC++準拠
	srvDesc_.Buffer.NumElements = instancingCount;
	instanceSrvHandleCPU_ = DescriptorHandIe::GetCPUHandle(srvHeap, srvSize, 127u);
	instanceSrvHandleGPU_ = DescriptorHandIe::GetGPUHandle(srvHeap, srvSize, 127u);
	device->CreateShaderResourceView(instancingData_.Get(), &srvDesc_, instanceSrvHandleCPU_);

	for (uint32_t i = 0u; i < instancingCount; ++i) {
		instancingArray_[i].World = Matrix4x4::Identity();
	}
}

void GameScene::OnExit() {}

void GameScene::Update() {

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();
	camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	//model_->ImGuiWidget();
	//transform_.ImGuiWidget();
	ImGui::End();

	TextureManager::GetInstance()->ImGuiWindow();

	light_->ImGuiWidget();

	transform_.UpdateMatrix();
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

#pragma region モデル描画

	Model::StartDraw(commandList);

	light_->SetLight(commandList);

	// モデルの描画
	model_->Draw(instanceSrvHandleGPU_, 5u, camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}