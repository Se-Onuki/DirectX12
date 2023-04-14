#include <Windows.h>
#include <cstdint>

// ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
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



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	OutputDebugStringA("Hello,DirectX!\n");

#pragma region ウィンドウクラス

	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"CG2_DirectX12";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する。
	RegisterClass(&wc);

#pragma endregion

#pragma region クライアント領域の設定

	// クライアント領域のサイズ
	const uint32_t kClientWidth = 1280;
	const uint32_t kClientHeight = 720;

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0, kClientWidth, kClientHeight };
	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

#pragma endregion

#pragma region ウィンドウ生成=>出力

	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		// 利用するクラス名
		L"DirectXGame",			// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示X座標 (windowsに任せる)
		CW_USEDEFAULT,			// 表示Y座標 (windowsに任せる)
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc.hInstance,			// インスタンスハンドル
		nullptr					// オプション
	);
	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

#pragma endregion

	MSG msg{};

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	return 0;
}