#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <cstdint>

#include <wrl.h>

#include <string>


#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#pragma comment(lib, "winmm.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class WinApp
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	WinApp() = default;
	WinApp(const WinApp &) = delete;
	const WinApp &operator=(const WinApp &) = delete;
	~WinApp() = default;

	RECT wrc;
	HWND hwnd;
	WNDCLASS wc{};	// ウィンドウクラス

	ComPtr<ID3D12Debug1> debugController;

public:

	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅
	static const wchar_t kWindowClassName[];

	static WinApp *const GetInstance();

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND GetHWND() const {
		return hwnd;
	}

	HINSTANCE GetHInstance() const {
		return wc.hInstance;
	}

	void CreateGameWindow(
		const char *title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	bool ProcessMessage() const;

	static void StaticInit();
	void Finalize();
};