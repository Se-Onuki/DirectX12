/// @file SolEngine.cpp
/// @brief エンジンの実装
/// @author ONUKI seiya
#include "SolEngine.h"
#include "DirectBase/Base/DirectXCommon.h"
#include "DirectBase/Base/TextureManager.h"
#include "DirectBase/Input/Input.h"
#include "DirectBase/Base/Audio.h"
#include "DirectBase/Base/ImGuiManager.h"
#include "DirectBase/Base/Shader.h"
#include "DirectBase/Model/Model.h"
#include "DirectBase/2D/Sprite.h"

namespace SolEngine {

	void Engine::StaticInit(const char *title, UINT windowStyle, int32_t clientWidth, int32_t clientHeight) {

		Engine *const instance = Engine::GetInstance();

#pragma region 基盤初期化

		WinApp::StaticInit();

		WinApp *const winApp = WinApp::GetInstance();
		winApp->CreateGameWindow(title, windowStyle, clientWidth, clientHeight);

		instance->dxCommon_ = DirectXCommon::GetInstance();
		instance->dxCommon_->Init(winApp);

#pragma endregion

		instance->commandList_ = instance->dxCommon_->GetCommandList();

		::TextureManager *const textureManager = ::TextureManager::GetInstance();

		Input *const input = Input::GetInstance();
		//const DirectInput *const directInput = DirectInput::GetInstance();

		Audio *const audio = Audio::GetInstance();

#pragma region その他初期化

		textureManager->Init(instance->dxCommon_->GetDevice(), instance->commandList_);
		::TextureManager::Load("white2x2.png");

		ImGuiManager::StaticInit(winApp->GetHWND(), instance->dxCommon_->GetDevice(), instance->dxCommon_->backBufferCount_, instance->dxCommon_->GetSRVHeap());

		Model::StaticInit();
		Sprite::StaticInit();
		audio->StaticInit();

		input->Init();

#pragma endregion

	}

}