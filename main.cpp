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

#include "DirectBase/Base/WinApp.h"
#include "DirectBase/Base/DirectXCommon.h"

#include "externals/DirectXTex/DirectXTex.h"

#include "DirectBase/Base/TextureManager.h"
#include "DirectBase/Base/Shader.h"
#include "Header/Model/Model.h"
#include "DirectBase/Base/ImGuiManager.h"

#include "Scene/SceneManager.h"
#include "Scene/GameScene.h"
#include "DirectBase/Input/Input.h"

#include "Header/File/GlobalVariables.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

#pragma region 基盤初期化

	WinApp::StaticInit();

	WinApp *const winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("DirectXGame");

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	dxCommon->Init(winApp);

#pragma endregion

	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	TextureManager *const textureManager = TextureManager::GetInstance();

	Input *const input = Input::GetInstance();
	const DirectInput *const directInput = DirectInput::GetInstance();

	Audio *const audio = Audio::GetInstance();

#pragma region その他初期化

	textureManager->Init(dxCommon->GetDevice(), commandList);
	TextureManager::Load("white2x2.png");

	ImGuiManager::StaticInit(winApp->GetHWND(), dxCommon->GetDevice(), dxCommon->backBufferCount, textureManager->GetSRVHeap());

	Shader::StaticInit();
	Model::StaticInit();
	Sprite::StaticInit();
	audio->StaticInit();

	input->Init();

#pragma endregion

	GlobalVariables *const gVariable = GlobalVariables::GetInstance();
	gVariable->LoadFile();

	// シーン管理クラス
	SceneManager *const sceneManager = SceneManager::GetInstance();
	sceneManager->ChangeScene(new GameScene);


	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage() || directInput->IsPress(DIK_ESCAPE)) break;

		// キーボードの更新
		input->Update();

		// ImGuiに新規フレームであると伝える
		ImGuiManager::StartFlame();


		///
		/// ↓ゲーム処理↓
		///



		gVariable->Update();
		// ゲームの処理
		sceneManager->Update();


		///
		/// ↑ゲーム処理↑
		///


		// ImGuiの内部コマンドを生成する
		ImGuiManager::CreateCommand();

		///
		/// ↓描画関連↓
		///

		dxCommon->StartDraw();

		textureManager->StartDraw();

		// 一般描画
		sceneManager->Draw();


		// ImGuiの描画
		ImGuiManager::Draw(commandList);

		dxCommon->EndDraw();

		///
		/// ↑描画関連↑
		///

	}

#pragma region 各種解放

	ImGuiManager::Finalize();

	audio->Finalize();

	dxCommon->Finalize();

	winApp->Finalize();

#pragma endregion

	return 0;
}