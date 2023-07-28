#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3d12sdklayers.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


struct DirectResourceLeakChecker {
	~DirectResourceLeakChecker() {

#ifdef _DEBUG

#pragma region Report Live Objects

		// リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}

#pragma endregion

#endif // DEBUG

	}
};

struct LeakChecker {
	static DirectResourceLeakChecker leakChecker;
};
//static const DirectResourceLeakChecker leakChecker{};