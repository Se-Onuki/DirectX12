#include "DirectBase/Base/LeakChecker.h"

#include <Windows.h>
#include <cstdint>

#include <stdio.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <string>
#include <format>

#include "DirectBase/Base/WinApp.h"

#include "externals/DirectXTex/DirectXTex.h"

#include "Header/String/String.hpp"
#include "Header/Render/Render.hpp"

#include "Header/Math/Math.hpp"

#include "Header/Math/Vector3.h"
#include "Header/Math/Vector4.h"

#include "Header/Math/Matrix3x3.h"
#include "Header/Math/Matrix4x4.h"

#include "Header/Math/Transform.h"

#include "Header/Texture/Texture.h"
#include "Header/Create/Create.h"
#include "Header/Descriptor/DescriptorHandIe.h"

#include <algorithm>

#include "Header/Model/Model.h"
#include "DirectBase/3D/ViewProjection/ViewProjection.h"
#include "DirectBase/Base/DirectXCommon.h"
#include "DirectBase/Base/TextureManager.h"
#include "DirectBase/Base/Shader.h"
#include "DirectBase/Base/ImGuiManager.h"
#include "DirectBase/3D/DirectionLight.h"
#include "DirectBase/2D/Sprite.h"

//template <typename T>using  Microsoft::WRL::ComPtr = Microsoft::WRL:: Microsoft::WRL::ComPtr<T>;
//using namespace Microsoft::WRL;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	WinApp::StaticInit();

	WinApp *const winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("DirectXGame");

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	dxCommon->Init(winApp);

	ID3D12GraphicsCommandList *const commandList_ = dxCommon->commandList_.Get();

	TextureManager *const textureManager = TextureManager::GetInstance();

	textureManager->Init(dxCommon->GetDevice(), commandList_);
	//uint32_t uvTex =
	TextureManager::Load("white2x2.png");


#pragma region DescriptorSize

#pragma endregion

#pragma region DescriptorHeap

	ID3D12DescriptorHeap *const srvDescriptorHeap = textureManager->GetSRVHeap();


#pragma endregion


	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む


#pragma region ImGuiの初期化

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(dxCommon->GetDevice(),
		(int32_t)dxCommon->backBuffers_.size(),
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

#pragma endregion

	// FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	Shader::StaticInit();
	Model::StaticInit();
	Sprite::StaticInit();


#pragma region Model

	std::unique_ptr<Model> model;
	std::unique_ptr<Model> model2;

	model.reset(Model::LoadObjFile("", "multiMaterial.obj"));
	model2.reset(Model::LoadObjFile("", "plane.obj"));
	Transform planeTransform{ Vector3::one(),Vector3::zero(),{-1.f,0.f,0.f} };
	planeTransform.InitResource();


#pragma endregion

#pragma region ViewProjection

	ViewProjection viewProjection;
	viewProjection.Init();

#pragma endregion

	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Light::Direction));

#pragma region Transformを使ってCBufferを更新する

	// Ball用のTransform
	Transform transformBall{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,5.f} };
	transformBall.InitResource();

#pragma endregion

#pragma region Resourceにデータを書き込む

	// 頂点リソースにデータを書き込む
	Light::Direction *lightData = nullptr;
	// 書き込むためのアドレスを取得
	lightResource->Map(0, nullptr, reinterpret_cast<void **>(&lightData));
	lightData->color = { 1.f, 1.f, 1.f, 1.f };
	lightData->direction = Vector3{ 0.f,-1.f,0.f }.Nomalize();
	lightData->intensity = 1.f;

#pragma endregion

	Sprite sprite;
	sprite.Init();


	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage()) break;

#pragma region ImGuiに新規フレームであると伝える
		ImGuiManager::StartFlame();
#pragma endregion

#pragma region ゲームの処理

		ImGui::Begin("Camera");
		ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, Angle::Dig2Rad);
		ImGui::DragFloat3("translate", &viewProjection.translation_.x, 0.1f);
		ImGui::End();

		ImGui::Begin("Obj0");
		transformBall.ImGuiWidget();
		ImGui::End();

		ImGui::Begin("Obj1");
		planeTransform.ImGuiWidget();
		ImGui::End();

		ImGui::Begin("UI");
		sprite.ImGuiWidget();
		ImGui::End();

		ImGui::Begin("Light");
		ImGui::ColorEdit4("Color", &lightData->color.x);
		ImGui::DragFloat3("Direction", &lightData->direction.x, 1.f / 255, -1, 1);
		ImGui::DragFloat("Brightness ", &lightData->intensity, 0.01f, 0, 1);
		ImGui::End();
		lightData->direction = lightData->direction.Nomalize();


		transformBall.UpdateMatrix();
		planeTransform.UpdateMatrix();


		viewProjection.UpdateMatrix();


		ImGui::Begin("model");
		model->ImGuiWidget();
		ImGui::End();

		ImGui::ShowDemoWindow();

#pragma endregion

#pragma region ImGuiの内部コマンドを生成する

		ImGui::Render();

#pragma endregion


#pragma region コマンドを積み込んで確定させる

		dxCommon->StartDraw();

		textureManager->StartDraw();

#pragma region コマンドを積む

		Sprite::StartDraw(commandList_);
		sprite.Draw();
		Sprite::EndDraw();

		Model::StartDraw(commandList_);

		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kLight, lightResource->GetGPUVirtualAddress());

		// Ballの描画
		model->Draw(transformBall, viewProjection);
		//model2->Draw(planeTransform, viewProjection);

		Model::EndDraw();

#pragma endregion

#pragma region ImGuiの描画

		// 実際のCommandListにImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_);

#pragma endregion
		dxCommon->EndDraw();

#pragma endregion

	}

#pragma region 各種解放

	ImGuiManager::Finalize();

	dxCommon->Finalize();

	winApp->Finalize();

#pragma endregion

	return 0;
}