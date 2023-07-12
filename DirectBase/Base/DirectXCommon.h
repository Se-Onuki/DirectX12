#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "WinApp.h"
#include <dxgi1_6.h>
#include <vector>

class DirectXCommon
{
	DirectXCommon() = default;
	DirectXCommon(const DirectXCommon &) = delete;
	const DirectXCommon &operator=(const DirectXCommon &) = delete;
	~DirectXCommon() = default;

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ウィンドウ管理
	WinApp *winApp_;

	// DirectX管理
	// 定数
	UINT64 fenceVal_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
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

	std::vector<ComPtr<ID3D12Resource>> backBuffers_;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_;

	ComPtr<ID3D12Resource> depthBuffer_;

	ComPtr<ID3D12Fence> fence_;

	ComPtr<ID3D12DescriptorHeap> dsvHeap_;
public:

	void Init(WinApp *winApp, int32_t backBufferWidth = WinApp::kWindowWidth,
		int32_t backBufferHeight = WinApp::kWindowHeight);

	ID3D12Device *const GetDevice() {
		return device_.Get();
	}

	static DirectXCommon *const GetInstance();
};