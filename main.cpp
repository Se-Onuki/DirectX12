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

#include "Scene/SceneManager.h"
#include "Scene/GameScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	WinApp::StaticInit();

	WinApp *const winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("DirectXGame");

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	dxCommon->Init(winApp);

	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	TextureManager *const textureManager = TextureManager::GetInstance();

	textureManager->Init(dxCommon->GetDevice(), commandList);
	TextureManager::Load("white2x2.png");

	ImGuiManager::StaticInit(winApp->GetHWND(), dxCommon->GetDevice(), (int32_t)dxCommon->backBuffers_.size(), textureManager->GetSRVHeap());

	Shader::StaticInit();
	Model::StaticInit();
	Sprite::StaticInit();

	// シーン管理クラス
	SceneManager *const sceneManager = SceneManager::GetInstance();
	sceneManager->ChangeScene(new GameScene);


	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage()) break;

#pragma region ImGuiに新規フレームであると伝える
		ImGuiManager::StartFlame();
#pragma endregion

#pragma region ゲームの処理

		sceneManager->Update();
#pragma endregion

#pragma region ImGuiの内部コマンドを生成する
		ImGuiManager::CreateCommand();
#pragma endregion


#pragma region コマンドを積み込んで確定させる

		dxCommon->StartDraw();

		textureManager->StartDraw();

		// 一般描画
		sceneManager->Draw();


		// ImGuiの描画
		ImGuiManager::Draw(commandList);

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