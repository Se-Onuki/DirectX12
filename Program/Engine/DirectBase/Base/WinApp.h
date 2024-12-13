/// @file WinApp.h
/// @brief ウィンドウの管理を行う
/// @author ONUKI seiya
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

	/// @brief シングルトンの取得
	/// @return インスタンス
	static WinApp *const GetInstance();

	/// @brief ウィンドウプロシージャ
	/// @param[in] hwnd ウィンドウハンドル
	/// @param[in] msg メッセージ
	/// @param[in] wParam
	/// @param[in] lParam
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/// @brief ウィンドウハンドルの取得
	/// @return ウィンドウハンドル
	HWND GetHWND() const {
		return hwnd;
	}

	/// @brief インスタンスハンドルの取得
	/// @return インスタンスハンドル
	HINSTANCE GetHInstance() const {
		return wc.hInstance;
	}

	/// @brief ウィンドウの生成
	/// @param[in] title タイトル
	/// @param[in] windowStyle ウィンドウスタイル
	/// @param[in] clientWidth クライアント領域の幅
	/// @param[in] clientHeight クライアント領域の高さ
	void CreateGameWindow(
		const char *title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	/// @brief メッセージ処理
	/// @return 終了メッセージが来たら 真
	/// @details windowsにメッセージが来てたら最優先で処理させる
	bool ProcessMessage() const;

	/// @brief 初期化
	static void StaticInit();
	/// @brief 終了
	void Finalize();
};