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

	ImGui::Text("Identity<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(Quaternion::Identity).c_str());
	ImGui::Text("quaternion_[0u].Conjugation()<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(quaternion_[0u].Conjugation()).c_str());
	ImGui::Text("quaternion_[0u].Inverse()<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(quaternion_[0u].Inverse()).c_str());
	ImGui::Text("quaternion_[0u].Normalize()<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(quaternion_[0u].Normalize()).c_str());

	ImGui::Text("quaternion_[0u] * quaternion_[1u]<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(quaternion_[0u] * quaternion_[1u]).c_str());
	ImGui::Text("quaternion_[1u] * quaternion_[0u]<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(quaternion_[1u] * quaternion_[0u]).c_str());

	ImGui::Text("quaternion_[0u].Length()<%s> :%s", SoLib::Traits<float>::Name, SoLib::to_string(quaternion_[0u].Length()).c_str());

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
