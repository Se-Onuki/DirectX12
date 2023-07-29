#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#include <stdint.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


struct DirectResourceLeakChecker {
private:
	// リークチェックの実行管理値
	static uint32_t loadCount;

	DirectResourceLeakChecker(const DirectResourceLeakChecker &) = delete;
	DirectResourceLeakChecker operator=(const DirectResourceLeakChecker &) = delete;

public:
	DirectResourceLeakChecker() {
		loadCount++;
	}
	~DirectResourceLeakChecker() {
		loadCount--;
#ifdef _DEBUG
		if (loadCount == 0) {
#pragma region Report Live Objects

			// リソースリークチェック
			Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}

#pragma endregion
		}
#endif // DEBUG

	}
};

const extern DirectResourceLeakChecker leakChecker;

//static const DirectResourceLeakChecker leakChecker{};