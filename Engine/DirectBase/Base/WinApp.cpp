#include "WinApp.h"
#include "../String/String.h"

const wchar_t WinApp::kWindowClassName[] = L"DirectXGame";


void WinApp::StaticInit()
{

#pragma region COMの初期化

	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma endregion

	timeBeginPeriod(1u);

}
void WinApp::Finalize()
{

	CloseWindow(hwnd);

	CoUninitialize();

}

WinApp *const WinApp::GetInstance()
{
	static WinApp instance;
	return &instance;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

#ifdef _DEBUG

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}

#endif // _DEBUG

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		break;
	default:
		// 標準のメッセージ処理を伝える
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void WinApp::CreateGameWindow(const char *title, UINT windowStyle, int32_t clientWidth, int32_t clientHeight)
{

#pragma region ウィンドウクラス

	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = kWindowClassName;
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する。
	RegisterClass(&wc);

#pragma endregion

#pragma region クライアント領域の設定

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	wrc = { 0,0, clientWidth, clientHeight };
	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, windowStyle, false);

#pragma endregion

#pragma region ウィンドウ生成=>出力

	// ウィンドウの生成
	hwnd = CreateWindow(
		wc.lpszClassName,				// 利用するクラス名
		ConvertString(title).c_str(),	// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,			// よく見るウィンドウスタイル
		CW_USEDEFAULT,					// 表示X座標 (windowsに任せる)
		CW_USEDEFAULT,					// 表示Y座標 (windowsに任せる)
		wrc.right - wrc.left,			// ウィンドウ横幅
		wrc.bottom - wrc.top,			// ウィンドウ縦幅
		nullptr,						// 親ウィンドウハンドル
		nullptr,						// メニューハンドル
		wc.hInstance,					// インスタンスハンドル
		nullptr							// オプション
	);

#pragma region デバッグレイヤー

#ifdef _DEBUG

	debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

#endif // DEBUG

#pragma endregion

	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

#pragma endregion

}

bool WinApp::ProcessMessage() const
{
	MSG msg{};
	// windowsにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.message == WM_QUIT;	// 終了メッセージが来たら 真
}
