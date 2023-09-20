#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <cstdint>

#include "DirectBase/Base/WinApp.h"

class DirectXCommon;

class SolEngine {
	SolEngine() = default;
	SolEngine(const SolEngine &) = delete;
	SolEngine operator=(const SolEngine &) = delete;
	SolEngine operator=(SolEngine &&) = delete;
	~SolEngine() = default;
public:

	static SolEngine *const GetInstance() {
		static SolEngine instance{};
		return &instance;
	}

	static void StaticInit(
		const char *title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = WinApp::kWindowWidth, int32_t clientHeight = WinApp::kWindowHeight);

	WinApp *const GetWinApp() const { return winApp_; }
	DirectXCommon *const GetDXCommon() const { return dxCommon_; }

private:

	WinApp *winApp_ = nullptr;
	DirectXCommon *dxCommon_ = nullptr;
	ID3D12GraphicsCommandList *commandList_ = nullptr;

};