/// @file DirectXCommon.cpp
/// @brief DirectXの基本処理を担うクラス
/// @author ONUKI seiya
#include "DirectXCommon.h"
#include "../../externals/DirectXTex/d3dx12.h"
#include "../Create/Create.h"
#include "../String/String.h"
#include "../Texture/Texture.h"
#include "TextureManager.h"
#include <format>
#include <thread>
#include "EngineObject.h"

DirectResourceLeakChecker DirectXCommon::leakChecker{}; // リークチェッカー

void DirectXCommon::Init(WinApp *winApp, int32_t backBufferWidth, int32_t backBufferHeight)
{
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;
	// DXGIとD3D12 Deviceを生成する
	InitDXGI_Device();
	// Command関連を生成する
	InitCommand();
	// SwapChainを生成する。
	CreateSwapChain();
	// RenderTagetを生成する
	CreateRenderTarget();

	CreateDepthStencile();

	CreateFence();

	descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	srvHeap_ = std::make_unique<DescHeapCbvSrvUav>(device_.Get(), srvCount_);

	fpsManager_ = std::make_unique<FPSManager>();
	fpsManager_->Init();

	// エンジンオブジェクトへ参照用データを転送する
	TransfarEngineObject();
}

void DirectXCommon::Finalize()
{
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	CloseHandle(fenceEvent);
}

DirectXCommon *const DirectXCommon::GetInstance()
{
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::StartDraw()
{

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

#pragma region TransitionBarrierを張る

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = backBuffers_[backBufferIndex].Get();
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBurrierを張る
	commandList_->ResourceBarrier(1, &barrier);

#pragma endregion
	//DefaultDrawReset();
}

void DirectXCommon::DefaultDrawReset(bool hasDsv)
{
	UINT backBufferIndex = GetBackIndex();

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvDescHeap_->GetHeap()->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, descriptorSizeRTV);

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion


	// 指定した色で画面全体をクリアする
	static const SoLib::Color::RGB4 clearColor = { 0.1f, 0.25f, 0.5f, 1.f }; // 青っぽい色。 RGBAの値

	DrawTargetReset(&rtvHandle, clearColor, hasDsv ? &dsvHandle : nullptr, viewport, scissorRect);
}

void DirectXCommon::SetFullscreenViewPort(D3D12_VIEWPORT *viewport, D3D12_RECT *scissorRect)
{

	// クライアント領域のサイズと一緒にして画面全体に表示
	*viewport = { 0.f, 0.f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.f, 1.f };

	// 基本的にビューポートと同じ短形が構成されるようにする
	scissorRect->left = 0;
	scissorRect->right = WinApp::kWindowWidth;
	scissorRect->top = 0;
	scissorRect->bottom = WinApp::kWindowHeight;
}

void DirectXCommon::DrawTargetReset(D3D12_CPU_DESCRIPTOR_HANDLE *rtvHandle, const SoLib::Color::RGB4 &clearColor, D3D12_CPU_DESCRIPTOR_HANDLE *dsvHandle, const D3D12_VIEWPORT &vp, const D3D12_RECT &scissorRect)
{
	commandList_->OMSetRenderTargets(1, rtvHandle, false, dsvHandle);
	if (dsvHandle) {
		commandList_->ClearDepthStencilView(*dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
	}
	if (rtvHandle) {
		commandList_->ClearRenderTargetView(*rtvHandle, clearColor, 0, nullptr);
	}
	commandList_->RSSetViewports(1, &vp);       // Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect); // Scirssorを設定
}

void DirectXCommon::EndDraw()
{
	HRESULT hr = S_FALSE;
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

#pragma region 画面状態の遷移

	// 画面に映す処理は全て終わり、画面に映すので、状態を遷移
	// 今回はRenderTargetからPresentにする
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers_[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

#pragma endregion

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからclearすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

#pragma region コマンドをキックする

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList *commandLists[] = { commandList_.Get() };
	commandQueue_.Get()->ExecuteCommandLists(1, commandLists);
	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

#pragma region GPUにシグナルを送る

	// Fenceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

#pragma endregion

#pragma region Fenceの値を確認してGPUを待つ

	// Fenceの値が指定したらSignal値にたどりついているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定したSignalに達していないので、たどり着くまで待つようにイベントを設定する。
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);
		// イベント待機
		WaitForSingleObject(fenceEvent, INFINITE);
	}

#pragma endregion

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_.Get()->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_.Get()->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

#pragma endregion

	TextureManager::GetInstance()->EndFlame();

	fpsManager_->Update();
}

void DirectXCommon::CrearDepthBuffer()
{

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void DirectXCommon::InitDXGI_Device()
{

#pragma region DXGIFactoryの生成

	dxgiFactory_ = nullptr;
	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判別できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 使用するアダプタ(GPU)を決定する
	// 使用するアダプタ用の変数。最初にnullptrを入れておく。
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	// 良い順にアダプタを積む
	for (UINT i = 0; dxgiFactory_.Get()->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得できないのは一大事
		// ソフトウェアアダプタでなければ、採用。
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。wstringなので注意
			Log(SolEngine::ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

#pragma endregion

#pragma region D3D12Deviceの生成

	device_ = nullptr;
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0 };
	const char *featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 採用した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			DirectXCommon::Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_.Get() != nullptr);
	DirectXCommon::Log("Complete create D3D121Device!!!\n"); // 初期化完了のログを出す

#pragma region エラー/警告時に停止

#ifdef _DEBUG

	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_.Get()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告の時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

#pragma region エラー/警告の抑制

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるメッセージ
			// https://stackoverflow.com/Questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する。
		infoQueue->PushStorageFilter(&filter);

#pragma endregion
	}

#endif // DEBUG

#pragma endregion

#pragma endregion
}

void DirectXCommon::InitCommand()
{
	HRESULT hr;
#pragma region CommandQueueを生成する
	// コマンドキューを生成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr = device_.Get()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	IID_PPV_ARGS(commandQueue_.GetAddressOf());
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region CommandListを生成する
	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr = device_.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	commandList_ = nullptr;
	hr = device_.Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion
}

void DirectXCommon::CreateSwapChain()
{

#pragma region SwapChainを生成する

	// スワップチェーンを生成する
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kWindowWidth;                  // 画面の幅。	ウィンドウクライアント領域と同じにしておく
	swapChainDesc.Height = WinApp::kWindowHeight;                // 画面の高さ。ウィンドウクライアント領域と同じにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // 色の設定
	swapChainDesc.SampleDesc.Count = 1;                          // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描写のターゲットとして利用する
	swapChainDesc.BufferCount = 2;                               // ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // モニタにうつしたら、中身を破壊
	// コマンドキュー。ウィンドウハンドル
	HRESULT hr = S_FALSE;
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHWND(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1 **>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion
}

void DirectXCommon::CreateRenderTarget()
{

	HRESULT hr;


	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	rtvDescHeap_ = std::make_unique<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>>(device_.Get(), backBufferCount_, false);
	//rtvHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, backBufferCount_, false);

#pragma region SwapChainからResourceを引っ張ってくる
	// SwapChainからResourceを引っ張ってくる
	// Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffers_[0].GetAddressOf()));
	// うまく取得できなければ生成できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(backBuffers_[1].GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region RTVを作る

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescHeap_->GetHeap()->GetCPUDescriptorHandleForHeapStart();

	auto handle = rtvDescHeap_->GetHandle(0, 0);

	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// まずは1つめを作る。1つ目は最初の所に作る。作る場所をこちらで指定してあげる必要がある。
	rtvHandles[0] = rtvDescHeap_->GetHandle(0, 0).cpuHandle_;
	device_->CreateRenderTargetView(backBuffers_[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを作る。
	rtvHandles[1] = rtvDescHeap_->GetHandle(0, 1).cpuHandle_;
	device_->CreateRenderTargetView(backBuffers_[1].Get(), &rtvDesc, rtvHandles[1]);

#pragma endregion
}

void DirectXCommon::CreateFence()
{
	HRESULT hr;
#pragma region FanceとEventを生成する

	// 初期値0でFanceを作る
	fence_ = nullptr;
	fenceValue_ = 0;
	hr = device_.Get()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = nullptr;
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

#pragma endregion
}

void DirectXCommon::TransfarEngineObject()
{
	SolEngine::EngineObject::device_ = device_.Get();
	SolEngine::EngineObject::srvHeap_ = srvHeap_.get();
	SolEngine::EngineObject::rtvHeap_ = rtvDescHeap_.get();
	SolEngine::EngineObject::commandList_ = commandList_.Get();

	SolEngine::EngineObject::descriptorSizeSRV_ = descriptorSizeSRV;
	SolEngine::EngineObject::descriptorSizeRTV_ = descriptorSizeRTV;
	SolEngine::EngineObject::descriptorSizeDSV_ = descriptorSizeDSV;

}

void DirectXCommon::CreateDepthStencile()
{

	depthBuffer_ = SolEngine::TextureFunc::CreateDepthStencilTextureResource(device_.Get(), WinApp::kWindowWidth, WinApp::kWindowHeight);
	dsvHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;        // Format。基本的にはResourceに合わせる。
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture
	// DSVHeapの先頭にDSVを構築する。
	device_.Get()->CreateDepthStencilView(depthBuffer_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::FPSManager::Init()
{
	reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::FPSManager::Update()
{
	static const std::chrono::microseconds kMinTime{ uint64_t(1000000.f / 60.f) };
	static const std::chrono::microseconds kMinCheckTime{ uint64_t(1000000.f / 65.f) };

	// 現在の時間
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	if (elapsed < kMinCheckTime) {
		// 経過するまで微小なスリープを行う
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	reference_ = std::chrono::steady_clock::now();
}
