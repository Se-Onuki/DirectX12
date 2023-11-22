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

	ImGui::Text("rotation<%s> :\n%s", SoLib::Traits<Quaternion>::Name, SoLib::to_string(rotation_).c_str());
	ImGui::Text("rotateMatrix<%s> :\n%s", SoLib::Traits<Matrix4x4>::Name, SoLib::to_string(rotateMatrix_).c_str());

	ImGui::Text("rotateByQuaternion<%s> :\n%s", SoLib::Traits<Vector3>::Name, SoLib::to_string(rotateByQuaternion_).c_str());
	ImGui::Text("rotateByMatrix<%s> :\n%s", SoLib::Traits<Vector3>::Name, SoLib::to_string(rotateByMatrix_).c_str());

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
