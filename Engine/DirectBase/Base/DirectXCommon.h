#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "WinApp.h"
#include <dxgi1_6.h>
#include <vector>
#include <array>

#include "LeakChecker.h"

class DirectXCommon {
	DirectXCommon() = default;
	DirectXCommon(const DirectXCommon &) = delete;
	const DirectXCommon &operator=(const DirectXCommon &) = delete;
	~DirectXCommon() = default;

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static DirectResourceLeakChecker leakChecker; // リークチェッカー
public:

	static const uint32_t backBufferCount = 2;

private:

	// ウィンドウ管理
	WinApp *winApp_ = nullptr;

	// DirectX管理
	// 定数
	UINT64 fenceValue_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	uint32_t descriptorSizeSRV = 0;
	uint32_t descriptorSizeRTV = 0;
	uint32_t descriptorSizeDSV = 0;

	// DXGI
	ComPtr<IDXGIFactory7> dxgiFactory_;

	// DirectXDevice
	ComPtr<ID3D12Device> device_;

	// コマンドリスト関係
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12CommandQueue> commandQueue_;

	// スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_;

	ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	std::array<ComPtr<ID3D12Resource>, backBufferCount> backBuffers_;

	ComPtr<ID3D12Fence> fence_;

	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> dsvHeap_;
public:

	void Init(WinApp *winApp, int32_t backBufferWidth = WinApp::kWindowWidth,
		int32_t backBufferHeight = WinApp::kWindowHeight);

	ID3D12Device *const GetDevice()const {
		return device_.Get();
	}
	ID3D12GraphicsCommandList *const GetCommandList()const {
		return commandList_.Get();
	}

	/// @brief 各種破棄
	void Finalize();

	static DirectXCommon *const GetInstance();

	static void Log(const std::string &message) {
		OutputDebugStringA(message.c_str());
	}

	void StartDraw();

	void EndDraw();

	void CrearDepthBuffer();


private:

	void InitDXGI_Device();

	void InitCommand();

	void CreateSwapChain();

	void CreateRenderTarget();

	void CreateDepthStencile();

	void CreateFence();
};