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

	for (uint32_t i = 0u; i < transformArray_.size(); ++i) {
		transformArray_[i].GetCBuffer()->SetMapAddress(&instanceTransform_[i].transform);
		colorArray_[i].SetMapAddress(&instanceTransform_[i].color);
		colorArray_[i] = Vector4{ 1.f,1.f,1.f,1.f };
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
		if (ImGui::TreeNode(("Particle" + std::to_string(i)).c_str())) {
			transformArray_[i].ImGuiWidget();
			ImGui::ColorEdit4("Color", &colorArray_[i]->x);
			ImGui::TreePop();
		}
		transformArray_[i].UpdateMatrix();
	}
	ImGui::End();

	TextureManager::GetInstance()->ImGuiWindow();

	light_->ImGuiWidget();

	transform_.ImGuiWidget();
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

	Model::SetPipelineType(Model::PipelineType::kModel);

	model_->Draw(transform_, camera_);
	Model::SetPipelineType(Model::PipelineType::kParticle);

	// モデルの描画
	model_->Draw(instanceTransform_, camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	Sprite::EndDraw();

#pragma endregion

}