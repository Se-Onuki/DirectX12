#include "Engine/DirectBase/Base/LeakChecker.h"

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

#include "Engine/SolEngine.h"

#include "Engine/DirectBase/Base/WinApp.h"
#include "Engine/DirectBase/Base/DirectXCommon.h"

#include "externals/DirectXTex/DirectXTex.h"

#include "Engine/DirectBase/Base/TextureManager.h"
#include "Engine/DirectBase/Base/Shader.h"
#include "Engine/DirectBase/Model/Model.h"
#include "Engine/DirectBase/Base/ImGuiManager.h"

#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"
#include "Scene/GameScene.h"
#include "Engine/DirectBase/Input/Input.h"
#include "Engine/DirectBase/Base/Audio.h"
#include "Engine/DirectBase/Texture/FullScreenTextureStrage.h"

#include "Engine/DirectBase/File/GlobalVariables.h"
#include "Header/Object/Particle/ParticleManager.h"

#include "Header/Object/Fade.h"
#include "Scene/CGTaskScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

#pragma region 基盤初期化

	WinApp::StaticInit();

	WinApp *const winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("LE3A_03_オヌキ_セイヤ");

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	dxCommon->Init(winApp);

#pragma endregion

	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	TextureManager *const textureManager = TextureManager::GetInstance();

	Input *const input = Input::GetInstance();
	//const DirectInput *const directInput = DirectInput::GetInstance();

	Audio *const audio = Audio::GetInstance();

#pragma region その他初期化

	textureManager->Init(dxCommon->GetDevice(), commandList);
	TextureManager::Load("white2x2.png");

	ImGuiManager::StaticInit(winApp->GetHWND(), dxCommon->GetDevice(), dxCommon->backBufferCount_, dxCommon->GetSRVHeap());

	Model::StaticInit();
	Sprite::StaticInit();
	audio->StaticInit();
	SolEngine::FullScreenTextureStrage::GetInstance()->Init();

	input->Init();

#pragma endregion

#pragma region クラス化の残骸

	//SolEngine::StaticInit("SoLEngine");

	//auto *const winApp = SolEngine::GetInstance()->GetWinApp();

	//auto *const dxCommon = SolEngine::GetInstance()->GetDXCommon();
	//ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	//Input *const input = Input::GetInstance();
	//const DirectInput *const directInput = DirectInput::GetInstance();

	//TextureManager *const textureManager = TextureManager::GetInstance();

	//Audio *const audio = Audio::GetInstance();

#pragma endregion

	// フェード演出の初期化
	Fade::StaticInit(); // 静的初期化
	Fade::GetInstance()->SetState({ 0.f,0.f }, 0x000000FF);
	Fade::GetInstance()->SetEaseFunc(SoLib::easeInQuad);

	GlobalVariables *const gVariable = GlobalVariables::GetInstance();
	gVariable->LoadFile();

	// シーン管理クラス
	SceneManager *const sceneManager = SceneManager::GetInstance();
	sceneManager->StaticInit();
	sceneManager->Init();
	sceneManager->ChangeScene("TitleScene", 0.f);

	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage() /*|| directInput->IsPress(DIK_ESCAPE)*/) { break; }

		// キーボードの更新
		input->Update();

		// ImGuiに新規フレームであると伝える
		ImGuiManager::StartFlame();

		const float deltaTime = ImGui::GetIO().DeltaTime;

		///
		/// ↓ゲーム処理↓
		///


#ifdef USE_IMGUI

		ImGui::Begin("Flame");
		ImGui::Text("FPS   : %f", ImGui::GetIO().Framerate);
		ImGui::Text("Delta : %f", deltaTime);

		ImGui::End();

		sceneManager->ImGuiWidget();
		audio->ImGuiWidget();
		textureManager->ImGuiWindow();

#endif // USE_IMGUI


		gVariable->Update();
		// ゲームの処理
		sceneManager->Update(deltaTime);


		///
		/// ↑ゲーム処理↑
		///


		// ImGuiの内部コマンドを生成する
		ImGuiManager::CreateCommand();

		///
		/// ↓描画関連↓
		///

		dxCommon->StartDraw();
		sceneManager->PostEffectSetup();

		textureManager->StartDraw();

		// 一般描画
		sceneManager->Draw();

		sceneManager->PostEffectEnd();

		// ImGuiの描画
		ImGuiManager::Draw(commandList);

		dxCommon->EndDraw();

		///
		/// ↑描画関連↑
		///

	}

#pragma region 各種解放

	//ParticleManager::GetInstance()->

	BlockManager::Finalize();
	SkinModelListManager::Finalize();
	SolEngine::ResourceObjectManager<Shader>::Finalize();
	SolEngine::ResourceObjectManager<RootSignature>::Finalize();
	SolEngine::FullScreenTextureStrage::Finalize();
	Sprite::Finalize();

	sceneManager->Finalize();

	ImGuiManager::Finalize();

	audio->Finalize();

	dxCommon->Finalize();

	winApp->Finalize();

#pragma endregion

	return 0;
}