#include "TitleScene.h"

#include <imgui.h>
#include "GameScene.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Utils/SoLib/SoLib.h"

TitleScene::TitleScene() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter() {

	light_.reset(DirectionLight::Create());
}

void TitleScene::OnExit() {
}

void TitleScene::Update() {

	rotateMat_[0u] = Matrix4x4::DirectionToDirection(Vector3{ 1.f,0.f,0.f }, Vector3{ -1.f,0.f,0.f });
	rotateMat_[1u] = Matrix4x4::DirectionToDirection(from_[0u], to_[0u]);
	rotateMat_[2u] = Matrix4x4::DirectionToDirection(from_[1u], to_[1u]);

	ImGui::Text("rotateMat_[0u]<%s> :\n%s", SoLib::Traits<Matrix4x4>::Name, SoLib::to_string(rotateMat_[0u]).c_str());
	ImGui::Text("rotateMat_[1u]<%s> :\n%s", SoLib::Traits<Matrix4x4>::Name, SoLib::to_string(rotateMat_[1u]).c_str());
	ImGui::Text("rotateMat_[2u]<%s> :\n%s", SoLib::Traits<Matrix4x4>::Name, SoLib::to_string(rotateMat_[2u]).c_str());

}

void TitleScene::Draw() {
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

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);



	Sprite::EndDraw();

#pragma endregion

}
