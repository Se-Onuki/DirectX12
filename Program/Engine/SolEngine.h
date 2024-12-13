/// @file SolEngine.h
/// @brief エンジンの実装
/// @author ONUKI seiya
#pragma once

#include <Windows.h>
#include <cstdint>
#include <d3d12.h>

#include "DirectBase/Base/WinApp.h"
#include "Utils/Containers/Singleton.h"

class DirectXCommon;

namespace SolEngine {

	class Engine : public SoLib::Singleton<Engine>
	{
		Engine() = default;
		Engine(const Engine &) = delete;
		Engine operator=(const Engine &) = delete;
		Engine operator=(Engine &&) = delete;
		~Engine() = default;

		friend SoLib::Singleton<Engine>;

	public:
		/// @brief エンジンの初期化
		static void StaticInit(
			const char *title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
			int32_t clientWidth = WinApp::kWindowWidth, int32_t clientHeight = WinApp::kWindowHeight);

		/// @brief ウィンドウの管理クラスの取得
		WinApp *const GetWinApp() const { return winApp_; }
		/// @brief DirectXCommonの取得
		DirectXCommon *const GetDXCommon() const { return dxCommon_; }

	private:
		WinApp *winApp_ = nullptr;
		DirectXCommon *dxCommon_ = nullptr;
		ID3D12GraphicsCommandList *commandList_ = nullptr;
	};
}