#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "WinApp.h"
#include <dxgi1_6.h>
#include <vector>
#include <array>
#include <memory>
#include <chrono>

#include "LeakChecker.h"

#include "../Descriptor/DescriptorManager.h"
#include "../../Engine/Utils/Graphics/Color.h"

class DirectXCommon {
	DirectXCommon() = default;
	DirectXCommon(const DirectXCommon &) = delete;
	const DirectXCommon &operator=(const DirectXCommon &) = delete;
	~DirectXCommon() = default;

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static DirectResourceLeakChecker leakChecker; // リークチェッカー
public:
	class FPSManager {
	public:
		FPSManager() = default;
		~FPSManager() = default;

		void Init();
		void Update();

	private:
		std::chrono::steady_clock::time_point reference_;
	};

	static const uint32_t backBufferCount_ = 2u;
	static const uint32_t srvCount_ = 512u;

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

	std::unique_ptr<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>> rtvDescHeap_;
	std::array<ComPtr<ID3D12Resource>, backBufferCount_> backBuffers_;

	ComPtr<ID3D12Fence> fence_;

	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> dsvHeap_;

	// SRVデスクリプタヒープ
	std::unique_ptr<DescHeapCbvSrvUav> srvHeap_;

	std::unique_ptr<FPSManager> fpsManager_ = nullptr;
public:

	void Init(WinApp *winApp, int32_t backBufferWidth = WinApp::kWindowWidth,
		int32_t backBufferHeight = WinApp::kWindowHeight);

	ID3D12Device *const GetDevice()const {
		return device_.Get();
	}
	ID3D12GraphicsCommandList *const GetCommandList()const {
		return commandList_.Get();
	}

	DescHeapCbvSrvUav *const GetSRVHeap() {
		return srvHeap_.get();
	}

	template <D3D12_DESCRIPTOR_HEAP_TYPE type>
	uint32_t GetHeapSize() {
		if constexpr (type == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
			return descriptorSizeSRV;
		}
		if constexpr (type == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
			return descriptorSizeRTV;
		}
		if constexpr (type == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
			return descriptorSizeDSV;
		}
		return 0;
	}

	/// @brief 各種破棄
	void Finalize();

	static DirectXCommon *const GetInstance();

	static void Log(const std::string &message) {
		OutputDebugStringA(message.c_str());
	}

	void StartDraw();

	/// @brief 初期設定で描画設定を行う
	/// @param hasDsv DSVを持っているか
	void DefaultDrawReset(bool hasDsv = true);

	/// @brief フルスクリーンのビューポート設定を適用する
	/// @param viewport ビューポート
	/// @param scissorRect シザー短形
	void SetFullscreenViewPort(D3D12_VIEWPORT *viewport, D3D12_RECT *scissorRect);

	void DrawTargetReset(D3D12_CPU_DESCRIPTOR_HANDLE *rtvHandle, const SoLib::Color::RGB4 &clearColor, D3D12_CPU_DESCRIPTOR_HANDLE *dsvHandle, const D3D12_VIEWPORT &vp, const D3D12_RECT &scissorRect);

	void EndDraw();

	void CrearDepthBuffer();

	/// @brief バックバッファのindexを取得する
	/// @return バックバッファのindex
	uint32_t GetBackIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }

	ID3D12DescriptorHeap *GetDsvDescHeap() { return dsvHeap_.Get(); }


private:

	void InitDXGI_Device();

	void InitCommand();

	void CreateSwapChain();

	void CreateRenderTarget();

	void CreateDepthStencile();

	void CreateFence();

	void TransfarEngineObject();
};