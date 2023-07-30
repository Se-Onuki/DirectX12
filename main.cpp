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

	ID3D12GraphicsCommandList *const commandList = dxCommon->commandList_.Get();

	TextureManager *const textureManager = TextureManager::GetInstance();

	textureManager->Init(dxCommon->GetDevice(), commandList);
	TextureManager::Load("white2x2.png");

	ImGuiManager::StaticInit(winApp->GetHWND(), dxCommon->GetDevice(), (int32_t)dxCommon->backBuffers_.size(), textureManager->GetSRVHeap());


	Shader::StaticInit();
	Model::StaticInit();
	Sprite::StaticInit();


#pragma region Model

	std::unique_ptr<Model> model{ Model::LoadObjFile("", "multiMaterial.obj") };
	std::unique_ptr<Model> model2{ Model::LoadObjFile("", "plane.obj") };

	Transform planeTransform{ Vector3::one(),Vector3::zero(),{-1.5f,0.f,0.f} };
	planeTransform.InitResource();


#pragma endregion

	ViewProjection viewProjection;
	viewProjection.Init();


	// Ball用のTransform
	Transform transformBall{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,5.f} };
	transformBall.InitResource();


	std::unique_ptr<DirectionLight> light{ DirectionLight::Create() };

	std::unique_ptr<Sprite> sprite{ Sprite::Create() };


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
		sprite->ImGuiWidget();
		ImGui::End();



		ImGui::Begin("Light");
		light->ImGuiWidget();
		ImGui::End();


		transformBall.UpdateMatrix();
		planeTransform.UpdateMatrix();


		viewProjection.UpdateMatrix();


		ImGui::Begin("model");
		model->ImGuiWidget();
		ImGui::End();

		ImGui::ShowDemoWindow();

#pragma endregion

#pragma region ImGuiの内部コマンドを生成する
		ImGuiManager::CreateCommand();
#pragma endregion


#pragma region コマンドを積み込んで確定させる

		dxCommon->StartDraw();

		textureManager->StartDraw();

#pragma region コマンドを積む

		Sprite::StartDraw(commandList);
		sprite->Draw();
		Sprite::EndDraw();

		Model::StartDraw(commandList);

		light->SetLight(commandList);

		// モデルの描画
		model->Draw(transformBall, viewProjection);
		model2->Draw(planeTransform, viewProjection);

		Model::EndDraw();

#pragma endregion

#pragma region ImGuiの描画
		ImGuiManager::Draw(commandList);

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