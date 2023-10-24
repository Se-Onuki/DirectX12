#include "GameScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"

#include "TitleScene.h"

#include "../Header/Entity/Component/ModelComp.h"
#include "../Utils/SoLib/SoLib_ImGui.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"

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
	auto *const srvHeap = DirectXCommon::GetInstance()->GetSRVHeap();

	//const uint32_t instancingCount = 5u;
	/*instancingData_ = CreateBufferResource(device, sizeof(Transform::TransformMatrix) * instancingCount);

	instancingData_->Map(0, nullptr, reinterpret_cast<void **>(&instancingArray_));*/

	//for (auto &transform : instanceTransform_) {
	//	transform.World = Matrix4x4::Identity();
	//}

	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	//srvDesc.Format = DXGI_FORMAT_UNKNOWN;	// 構造体の形は不明であるため
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	// textureではなくbufferとして使うため
	//srvDesc.Buffer.FirstElement = 0;
	//srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	//srvDesc.Buffer.StructureByteStride = sizeof(Transform::TransformMatrix);	// アライメントはC++準拠
	//srvDesc.Buffer.NumElements = instancingCount;

	const auto &heapRange = srvHeap->RequestHeapAllocation(1u);
	const auto &heapHandle = heapRange.GetHandle(0u);

	instanceSrvHandleCPU_ = heapHandle.cpuHandle_;
	instanceSrvHandleGPU_ = heapHandle.gpuHandle_;
	device->CreateShaderResourceView(instanceTransform_.GetResources(), &instanceTransform_.GetDesc(), instanceSrvHandleCPU_);

	for (uint32_t i = 0u; i < transformArray_.size(); ++i) {
		transformArray_[i].GetCBuffer()->SetMapAddress(&instanceTransform_[i]);
	}
}

void GameScene::OnExit() {}

void GameScene::Update() {

	ImGui::Begin("Camera");
	camera_.ImGuiWidget();
	ImGui::End();
	camera_.UpdateMatrix();

	ImGui::Begin("Sphere");
	model_->ImGuiWidget();
	for (uint32_t i = 0u; i < transformArray_.size(); ++i) {
		if (ImGui::TreeNode(("Transform" + std::to_string(i)).c_str())) {
			transformArray_[i].ImGuiWidget();
			ImGui::TreePop();
		}
		transformArray_[i].UpdateMatrix();
	}
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
	model_->Draw(instanceSrvHandleGPU_, instanceTransform_.size(), camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}