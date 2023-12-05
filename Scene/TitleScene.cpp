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

	std::array<Quaternion, 2u> rotation{
		Quaternion::AnyAxisRotation({0.71f,0.71f,0.f},0.3f),
		Quaternion::AnyAxisRotation({0.71f,0.f,0.71f},Angle::PI),
	};

	ImGui::Text("Slerp0<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(Quaternion::Slerp(rotation[0], rotation[1], 0.f)).c_str());
	ImGui::Text("Slerp1<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(Quaternion::Slerp(rotation[0], rotation[1], 0.3f)).c_str());
	ImGui::Text("Slerp2<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(Quaternion::Slerp(rotation[0], rotation[1], 0.5f)).c_str());
	ImGui::Text("Slerp3<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(Quaternion::Slerp(rotation[0], rotation[1], 0.7f)).c_str());
	ImGui::Text("Slerp4<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(Quaternion::Slerp(rotation[0], rotation[1], 1.f)).c_str());

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
