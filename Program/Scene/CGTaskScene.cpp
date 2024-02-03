#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/Model.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "CGTaskScene.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

void CGTaskScene::OnEnter()
{
	auto sphere = ModelManager::GetInstance()->AddModel("Sphere", Model::LoadObjFile("", "Sphere.obj"));

	CameraManager::GetInstance()->Init();
	light_ = DirectionLight::Create();
	{
		gameObject_ = std::make_unique<GameObject>();
		gameObject_->Init();
		auto modelComp = gameObject_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", sphere);

		gameObject_->transform_.translate = { 0.f,0.f,0.f };
		gameObject_->transform_.rotate = { 0.f,90._deg,0.f };
	}

}

void CGTaskScene::OnExit()
{
}

void CGTaskScene::Update()
{
	const float deltaTime = ImGui::GetIO().DeltaTime;

	light_->ImGuiWidget();
	gameObject_->ImGuiWidget();
	gameObject_->Update(deltaTime);

	CameraManager::GetInstance()->DisplayImGui();
	CameraManager::GetInstance()->Update(deltaTime);
}

void CGTaskScene::Draw()
{

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
	const auto &camera = *CameraManager::GetInstance()->GetUseCamera();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kModel);

	light_->SetLight(commandList);
	gameObject_->Draw(camera);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}
