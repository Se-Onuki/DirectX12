#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/Model.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "CGTaskScene.h"
#include "../Engine/DirectBase/Render/CameraManager.h"
#include "../../../externals/DirectXTex/d3dx12.h"
#include "../Engine/LevelEditor/LevelData.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../Engine/DirectBase/Model/AssimpData.h"
#include "../Engine/DirectBase/Model/Mesh.h"
#include "../Engine/DirectBase/Model/ModelData.h"
#include "../Engine/DirectBase/DxResource/DxResourceBufferPool.h"

void CGTaskScene::OnEnter()
{

	SolEngine::ResourceObjectManager<SolEngine::AssimpData> *const assimpManager = SolEngine::ResourceObjectManager<SolEngine::AssimpData>::GetInstance();
	auto assimpHandle = assimpManager->Load({ "Model/human/", "BrainStem.glb" });

	SolEngine::ResourceObjectManager<SolEngine::ModelData> *const modelDataManager = SolEngine::ResourceObjectManager<SolEngine::ModelData>::GetInstance();
	//SolEngine::ResourceObjectManager<SolEngine::Skeleton> *const skeletonManager = SolEngine::ResourceObjectManager<SolEngine::Skeleton>::GetInstance();
	boxModel_ = modelDataManager->Load({ assimpHandle });
	/*skeleton_ = skeletonManager->Load({ assimpHandle });
	skinModel_ = SkinModel::MakeSkinModel(*boxModel_, *skeleton_);*/

	pDxCommon_ = DirectXCommon::GetInstance();

	light_ = DirectionLight::Generate();
	SolEngine::CameraManager::GetInstance()->Init();
	auto *nowCamera = SolEngine::CameraManager::GetInstance()->GetCamera();
	nowCamera->translation_ = { 0.f, 0.9f,-2.5f };

	SolEngine::SkyBoxRender::GetInstance()->Init();

	skyBox_ = std::make_unique<SolEngine::SkyBox>();
	skyBox_->SetTexture("rostock_laage_airport_4k.dds");

	skyBoxTransform_->scale = Vector3::one * 100.f;

}

void CGTaskScene::OnExit()
{
	SolEngine::SkyBoxRender::Finalize();
	//fullScreen_->Finalize();
}

void CGTaskScene::Update()
{
	//const float deltaTime = ImGui::GetIO().DeltaTime;

	SolEngine::ResourceObjectManager<Shader>::GetInstance()->ImGuiWidget("ShaderManager");

	light_->ImGuiWidget();
	//model_->ImGuiWidget();
	transform_->ImGuiWidget();
	transform_->UpdateMatrix();

	skyBoxTransform_->ImGuiWidget("SkyBox");
	skyBoxTransform_->UpdateMatrix();

	//animationPlayer_.Update(deltaTime);

	auto material = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance()->ImGuiWidget("MaterialManager");
	if (material) {
		SoLib::ImGuiWidget("Material", *material);
	}


	//skinModel_->Update(**animation_, animationPlayer_.GetDeltaTimer().GetNowFlame());

	//SolEngine::CameraManager::GetInstance()->DisplayImGui();
	SolEngine::CameraManager::GetInstance()->CalcAll();

	//computeShader_.Update(*skinModel_->skinCluster_, *boxModel_, *skeleton_->modelInfluence_);
}

void CGTaskScene::Draw()
{

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
	const auto &camera = *SolEngine::CameraManager::GetInstance()->GetCamera();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

	SolEngine::SkyBoxRender::GetInstance()->Draw(*skyBox_, skyBoxTransform_, camera);

#pragma region モデル描画

	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kModel);
	Model::SetSkyBox(*skyBox_);

	light_->SetLight(commandList);
	boxModel_->Draw(transform_, camera);

	//model_->Draw(*skinModel_->skinCluster_, transform_, camera);
	//uvModel_->Draw(transform_, camera);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion

}

void CGTaskScene::PostEffectSetup()
{

	//	// 描画先のRTVとDSVを設定する
	//	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDxCommon_->GetDsvDescHeap()->GetCPUDescriptorHandleForHeapStart();
	//	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = offScreen_->GetRtvDescHeap()->GetHeap()->GetCPUDescriptorHandleForHeapStart();
	//
	//#pragma region ViewportとScissor(シザー)
	//
	//	// ビューポート
	//	D3D12_VIEWPORT viewport;
	//	// シザー短形
	//	D3D12_RECT scissorRect{};
	//
	//	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);
	//
	//#pragma endregion
	//
	//	pDxCommon_->DrawTargetReset(&rtvHandle, offScreen_->GetClearColor(), &dsvHandle, viewport, scissorRect);



	pDxCommon_->DefaultDrawReset(true);
}

void CGTaskScene::PostEffectEnd()
{

	//pDxCommon_->DefaultDrawReset(false);

	//fullScreen_->Draw({ L"FullScreen.VS.hlsl",L"FullScreen.PS.hlsl" }, offScreen_->GetResource(), offScreen_->GetHeapRange()->GetHandle(0).gpuHandle_);


}
