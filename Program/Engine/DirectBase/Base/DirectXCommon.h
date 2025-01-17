/// @file DirectXCommon.h
/// @brief DirectXの基本処理を担うクラス
/// @author ONUKI seiya
#pragma once
#include "WinApp.h"
#include <array>
#include <chrono>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <vector>
#include <wrl.h>

#include "LeakChecker.h"

#include "../../Engine/Utils/Graphics/Color.h"
#include "../Descriptor/DescriptorManager.h"

class DirectXCommon
{
	DirectXCommon() = default;
	DirectXCommon(const DirectXCommon &) = delete;
	const DirectXCommon &operator=(const DirectXCommon &) = delete;
	~DirectXCommon() = default;

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	static DirectResourceLeakChecker leakChecker; // リークチェッカー
public:
	/// @class FPSManager
	/// @brief FPS管理
	/// @details FPSの上限を管理する
	class FPSManager
	{
	public:
		FPSManager() = default;
		~FPSManager() = default;

		void Init();

		/// @brief 更新
		/// @details 60FPSを上限にする
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
	/// @fn void Init(void)
	/// @brief 初期化
	/// @param[in] winApp ウィンドウ管理
	/// @param[in] backBufferWidth ウィンドウサイズの幅
	/// @param[in] backBufferHeight ウィンドウサイズの高さ
	void Init(WinApp *winApp, int32_t backBufferWidth = WinApp::kWindowWidth,
			  int32_t backBufferHeight = WinApp::kWindowHeight);

	/// @fn ID3D12Device *const GetDevice(void)
	/// @brief D3D12デバイスの取得
	/// @return D3D12デバイス
	ID3D12Device *const GetDevice() const
	{
		return device_.Get();
	}

	/// @fn ID3D12GraphicsCommandList *const GetCommandList(void)
	/// @brief コマンドリストの取得
	/// @return コマンドリスト
	ID3D12GraphicsCommandList *const GetCommandList() const
	{
		return commandList_.Get();
	}

	/// @fn DescHeapCbvSrvUav *const GetSRVHeap(void)
	/// @brief SRVデスクリプタヒープの取得
	/// @return SRVデスクリプタヒープ
	DescHeapCbvSrvUav *const GetSRVHeap()
	{
		return srvHeap_.get();
	}

	/// @fn uint32_t GetHeapSize<D3D12_DESCRIPTOR_HEAP_TYPE>(void)
	/// @brief ヒープサイズの取得
	/// @tparam type ヒープタイプ
	/// @return 1個のヒープの幅
	template <D3D12_DESCRIPTOR_HEAP_TYPE type>
	uint32_t GetHeapSize()
	{
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

	/// @fn void Finalize(void)
	/// @brief 各種破棄
	void Finalize();

	/// @fn static DirectXCommon *const GetInstance(void)
	/// @brief シングルトンインスタンス取得
	/// @return インスタンス
	static DirectXCommon *const GetInstance();

	/// @fn static void Log(const std::string &)
	/// @brief ログ出力
	/// @param[in] message ログメッセージ
	static void Log(const std::string &message)
	{
		OutputDebugStringA(message.c_str());
	}

	/// @fn void StartDraw(void)
	/// @brief 描画開始
	/// @details 書き込み状態に変更する
	void StartDraw();

	/// @fn void DefaultDrawReset(bool)
	/// @brief 初期設定で描画設定を行う
	/// @param[in] hasDsv DSVを持っているか
	void DefaultDrawReset(bool hasDsv = true);

	/// @fn void SetFullscreenViewPort(D3D12_VIEWPORT *, D3D12_RECT *)
	/// @brief フルスクリーンのビューポート設定を適用する
	/// @param[out] viewport ビューポート
	/// @param[out] scissorRect シザー短形
	void SetFullscreenViewPort(D3D12_VIEWPORT *viewport, D3D12_RECT *scissorRect);

	/// @fn void DrawTargetReset(D3D12_CPU_DESCRIPTOR_HANDLE *, const SoLib::Color::RGB4 &, D3D12_CPU_DESCRIPTOR_HANDLE *, const D3D12_VIEWPORT &, const D3D12_RECT &)
	/// @brief 描画先のRTVとDSVを設定する
	/// @param[in] rtvHandle 描画先のRTVヒープのハンドル
	/// @param[in] clearColor 初期化する色
	/// @param[in] dsvHandle 描画先のDSVヒープのハンドル
	/// @param[in] vp ビューポート
	/// @param[in] scissorRect シザー短形
	void DrawTargetReset(D3D12_CPU_DESCRIPTOR_HANDLE *rtvHandle, const SoLib::Color::RGB4 &clearColor, D3D12_CPU_DESCRIPTOR_HANDLE *dsvHandle, const D3D12_VIEWPORT &vp, const D3D12_RECT &scissorRect);

	/// @fn void EndDraw(void)
	/// @brief 描画終了
	/// @details 書き込み状態を解除する
	void EndDraw();

	/// @fn void CrearDepthBuffer(void)
	/// @brief 深度バッファを初期化する
	void CrearDepthBuffer();

	/// @fn uint32_t GetBackIndex(void)
	/// @brief バックバッファのindexを取得する
	/// @return バックバッファのindex
	uint32_t GetBackIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }

	/// @fn ID3D12DescriptorHeap *GetDsvDescHeap(void)
	/// @brief DSVディスクリプタヒープの取得
	/// @return DSVディスクリプタヒープ
	ID3D12DescriptorHeap *GetDsvDescHeap() { return dsvHeap_.Get(); }

private:
	/// @fn void InitDXGI_Device(void)
	/// @brief DXGIの初期化
	void InitDXGI_Device();

	/// @fn void InitCommand(void)
	/// @brief コマンドリストの初期化
	void InitCommand();

	/// @fn void CreateSwapChain(void)
	/// @brief スワップチェインの初期化
	void CreateSwapChain();

	/// @fn void CreateRenderTarget(void)
	/// @brief レンターターゲットの生成
	void CreateRenderTarget();

	/// @fn void CreateDepthStencile(void)
	/// @brief 深度バッファの生成
	void CreateDepthStencile();

	/// @fn void CreateFence(void)
	/// @brief フェンスの生成
	void CreateFence();

	/// @fn void TransfarEngineObject(void)
	/// @brief エンジンオブジェクトにデータを転送する
	void TransfarEngineObject();
};