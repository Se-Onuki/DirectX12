#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/Model.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../Header/Entity/Component/ModelComp.h"
#include "CGTaskScene.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../externals/DirectXTex/d3dx12.h"
#include "../Engine/LevelEditor/LevelData.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../../ECS/Entity/EntityManager.hpp"
#include "../Engine/DirectBase/Model/AssimpData.h"
#include "../Engine/DirectBase/Model/Mesh.h"

void CGTaskScene::OnEnter()
{
	ModelManager::GetInstance()->AddModel("Block", Model::LoadAssimpModelFile("", "box.obj"));

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "test.json" });


	world_ = std::make_unique<World>();
	entityManager_ = world_->GetEntityManager();

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, world_.get());

	SolEngine::ResourceObjectManager<SolEngine::AssimpData> *const assimpManager = SolEngine::ResourceObjectManager<SolEngine::AssimpData>::GetInstance();
	auto boxHandle = assimpManager->Load({ "", "box.obj" });

	SolEngine::ResourceObjectManager<SolEngine::Material> *const materialManager = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance();

	// ↓ユーザが読み込みを行うとかいう最悪なコード
	for (uint32_t i = 0; i < boxHandle->importer_->GetScene()->mNumMaterials; i++) {
		materialManager->Load({ boxHandle, i });
	}
	
	SolEngine::ResourceObjectManager<SolEngine::Mesh> *const meshManager = SolEngine::ResourceObjectManager<SolEngine::Mesh>::GetInstance();

	// ↓ユーザが読み込みを行うとかいう最悪なコード
	for (uint32_t i = 0; i < boxHandle->importer_->GetScene()->mNumMeshes; i++) {
		meshManager->Load({ boxHandle, i });
	}




	model_ = ModelManager::GetInstance()->AddModel("HumanModel", Model::LoadAssimpModelFile("Model/human/", "sneakWalk.gltf"));

	uvModel_ = ModelManager::GetInstance()->AddModel("UvPlane", Model::LoadAssimpModelFile("", "plane.gltf"));

	pDxCommon_ = DirectXCommon::GetInstance();

	light_ = DirectionLight::Create();
	CameraManager::GetInstance()->Init();
	CameraManager::GetInstance()->GetUseCamera()->translation_.z = -15.f;

	offScreen_ = PostEffect::OffScreenRenderer::GetInstance();
	offScreen_->Init();

	fullScreen_ = PostEffect::FullScreenRenderer::GetInstance();
	fullScreen_->Init({ { L"FullScreen.VS.hlsl",L"FullScreen.PS.hlsl" } });

	// アニメーションを設定
	animation_ = ModelAnimation::Animation::CreateFromFile("Model/human/", "sneakWalk.gltf");
	animationPlayer_.SetAnimation(&animation_);
	animationPlayer_.Start(true);

	skinModel_ = SkinModel::MakeSkinModel(model_);

	vec2_ = std::make_unique<AlignasWrapper<Vector2>>();

	AlignasWrapper<Vector2, 8> test{ 53, 14 };

	*vec2_ = Vector2{ 10,10 };

	/*
	{
		gameObject_ = std::make_unique<GameObject>();
		gameObject_->Init();
		auto modelComp = gameObject_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", sphere);

		gameObject_->transform_.translate = { 0.f,0.f,0.f };
		gameObject_->transform_.rotate = { 0.f,90._deg,0.f };
	}*/

}

void CGTaskScene::OnExit()
{
	offScreen_->Finalize();
	fullScreen_->Finalize();
}

void CGTaskScene::Update()
{
	const float deltaTime = ImGui::GetIO().DeltaTime;

	SolEngine::ResourceObjectManager<Shader>::GetInstance()->ImGuiWidget("ShaderManager");

	light_->ImGuiWidget();
	model_->ImGuiWidget();
	transform_->ImGuiWidget();
	transform_->UpdateMatrix();

	animationPlayer_.Update(deltaTime, model_);
	//gameObject_->Update(deltaTime);

	skinModel_->Update(animation_, animationPlayer_.GetDeltaTimer().GetNowFlame());

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
	Model::SetPipelineType(Model::PipelineType::kSkinModel);

	light_->SetLight(commandList);
	model_->Draw(*skinModel_->skinCluster_, transform_, camera);
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

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDxCommon_->GetDsvDescHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = offScreen_->GetRtvDescHeap()->GetHeap()->GetCPUDescriptorHandleForHeapStart();

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

	pDxCommon_->DrawTargetReset(&rtvHandle, offScreen_->GetClearColor(), &dsvHandle, viewport, scissorRect);
}

void CGTaskScene::PostEffectEnd()
{

	pDxCommon_->DefaultDrawReset(false);

	fullScreen_->Draw({ L"FullScreen.VS.hlsl",L"FullScreen.PS.hlsl" }, offScreen_->GetTexture(), offScreen_->GetHeapRange()->GetHandle(0).gpuHandle_);


}
