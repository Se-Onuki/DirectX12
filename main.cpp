#include <Windows.h>
#include <cstdint>

#include <stdio.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <string>
#include <format>

#include "DirectBase/Base/WinApp.h"
//#include "externals/imgui/imgui.h"
//#include "externals/imgui/imgui_impl_dx12.h"
//#include "externals/imgui/imgui_impl_win32.h"
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"

#include "Header/String/String.hpp"
#include "Header/Render/Render.hpp"

#include "Header/Math/Math.hpp"

#include "Header/Math/Vector3.h"
#include "Header/Math/Vector4.h"

#include "Header/Math/Matrix3x3.h"
#include "Header/Math/Matrix4x4.h"

#include "Header/Math/Transform.h"

#include "Header/Texture/Texture.h"
#include "Header/Create/Create.h"
#include "Header/Descriptor/DescriptorHandIe.h"

#include <algorithm>
#include "Header/Model/Model.h"

//template <typename T>using  Microsoft::WRL::ComPtr = Microsoft::WRL:: Microsoft::WRL::ComPtr<T>;
//using namespace Microsoft::WRL;

struct DirectResourceLeakChecker {
	~DirectResourceLeakChecker() {

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
};

void Log(const std::string &message) {
	OutputDebugStringA(message.c_str());
}

Microsoft::WRL::ComPtr<IDxcBlob> const CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring &file_path,
	// Compilerに使用するProfile
	const wchar_t *profile,
	// 初期化して生成したものを3つ
	IDxcUtils *dxcUtils,
	IDxcCompiler3 *dxcCompiler,
	IDxcIncludeHandler *includeHandler
) {

#pragma region 1. hlslファイルを読む

	// これからシェーダをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", file_path, profile)));
	// hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding>shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(file_path.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

#pragma endregion

#pragma region 2. Compileする

	LPCWSTR arguments[] = {
		file_path.c_str(),			// コンパイル対象のhlslファイル名
		L"-E", L"main",				// エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile,				// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",						// 最適化を外しておく
		L"-Zpr",					// メモリレイアウトは行優先
	};

	// 実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		// 読み込んだファイル
		arguments,					// コンパイルオプション
		_countof(arguments),		// コンパイルオプションの数
		includeHandler,				// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	// コンパイル結果
	);

	// コンパイルエラーではなくdxcが起動できないなど致命的な状態
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 3. 警告・エラーがでていないか確認する

	// 警告・エラーが出てたらログに出して止める
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		// 警告・エラー、ダメゼッタイ
		assert(false);
	}

#pragma endregion

#pragma region 4. Compile結果を受け取って返す

	// コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path: {}, profile: {}\n", file_path, profile)));
	//実行用のバイナリを返却
	return shaderBlob;

#pragma endregion


}

// ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}
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

	DirectResourceLeakChecker leakChecker{};

#pragma region COMの初期化

	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma endregion

	WinApp *winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("DirectXGame");
	//
	//#pragma region ウィンドウクラス
	//
	//	WNDCLASS wc{};
	//	// ウィンドウプロシージャ
	//	wc.lpfnWndProc = WindowProc;
	//	// ウィンドウクラス名
	//	wc.lpszClassName = L"CG2_DirectX12";
	//	// インスタンスハンドル
	//	wc.hInstance = GetModuleHandle(nullptr);
	//	// カーソル
	//	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//
	//	// ウィンドウクラスを登録する。
	//	RegisterClass(&wc);
	//
	//#pragma endregion
	//
	//#pragma region クライアント領域の設定
	//
	//	// クライアント領域のサイズ
	//	const uint32_t kClientWidth = 1280;
	//	const uint32_t kClientHeight = 720;
	//
	//	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	//	RECT wrc = { 0,0, kClientWidth, kClientHeight };
	//	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	//	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	//
	//#pragma endregion
	//
	//#pragma region ウィンドウ生成=>出力
	//
	//	// ウィンドウの生成
	//	HWND hwnd = CreateWindow(
	//		wc.lpszClassName,		// 利用するクラス名
	//		L"DirectXGame",			// タイトルバーの文字
	//		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
	//		CW_USEDEFAULT,			// 表示X座標 (windowsに任せる)
	//		CW_USEDEFAULT,			// 表示Y座標 (windowsに任せる)
	//		wrc.right - wrc.left,	// ウィンドウ横幅
	//		wrc.bottom - wrc.top,	// ウィンドウ縦幅
	//		nullptr,				// 親ウィンドウハンドル
	//		nullptr,				// メニューハンドル
	//		wc.hInstance,			// インスタンスハンドル
	//		nullptr					// オプション
	//	);
	//
	//#pragma region デバッグレイヤー
	//
	//#ifdef _DEBUG
	//
	//	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
	//		// デバッグレイヤーを有効化する
	//		debugController->EnableDebugLayer();
	//		// さらにGPU側でもチェックを行うようにする
	//		debugController->SetEnableGPUBasedValidation(TRUE);
	//	}
	//
	//#endif // DEBUG
	//
	//#pragma endregion
	//
	//	// ウィンドウを表示する
	//	ShowWindow(hwnd, SW_SHOW);
	//
	//#pragma endregion

#pragma region DXGIFactoryの生成

	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory = nullptr;
	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判別できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 使用するアダプタ(GPU)を決定する
	// 使用するアダプタ用の変数。最初にnullptrを入れておく。
	Microsoft::WRL::ComPtr<IDXGIAdapter4 >useAdapter = nullptr;
	// 良い順にアダプタを積む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得できないのは一大事
		// ソフトウェアアダプタでなければ、採用。
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。wstringなので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

#pragma endregion

#pragma region D3D12Deviceの生成

	Microsoft::WRL::ComPtr<ID3D12Device>device = nullptr;
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1 ,D3D_FEATURE_LEVEL_12_0
	};
	const char *featureLevelStrings[] = { "12.2","12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		// 採用した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D121Device!!!\n"); // 初期化完了のログを出す

#pragma region エラー/警告時に停止

#ifdef _DEBUG

	Microsoft::WRL::ComPtr<ID3D12InfoQueue >infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告の時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

#pragma region エラー/警告の抑制

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるメッセージ
			// https://stackoverflow.com/Questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
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


		// 解放
		//infoQueue->Release();
	}

#endif // DEBUG

#pragma endregion

#pragma endregion

#pragma region DescriptorSize

	const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

#pragma endregion

#pragma region CommandQueueを生成する
	// コマンドキューを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	IID_PPV_ARGS(commandQueue.GetAddressOf());
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region CommandListを生成する
	// コマンドアロケータを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region SwapChainを生成する

	// スワップチェーンを生成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4>swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kWindowWidth;								// 画面の幅。	ウィンドウクライアント領域と同じにしておく
	swapChainDesc.Height = WinApp::kWindowHeight;							// 画面の高さ。ウィンドウクライアント領域と同じにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の設定
	swapChainDesc.SampleDesc.Count = 1;								// マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描写のターゲットとして利用する
	swapChainDesc.BufferCount = 2;									// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を破壊
	//コマンドキュー。ウィンドウハンドル
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), winApp->GetHWND(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1 **>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region DescriptorHeap

	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRV用のディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

#pragma endregion

#pragma region SwapChainからResourceを引っ張ってくる
	// SwapChainからResourceを引っ張ってくる
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(swapChainResources[0].GetAddressOf()));
	// うまく取得できなければ生成できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(swapChainResources[1].GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region RTVを作る

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// まずは1つめを作る。1つ目は最初の所に作る。作る場所をこちらで指定してあげる必要がある。
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを作る。
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

#pragma endregion

#pragma region ImGuiの初期化

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(device.Get(),
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

#pragma endregion

#pragma region FanceとEventを生成する

	// 初期値0でFanceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

#pragma endregion

#pragma region DXCの初期化

	// dxcCompilerを初期化
	Microsoft::WRL::ComPtr<IDxcUtils>dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region PSO(Pipeline State Object)

#pragma region RootSigneture(ルートシグネチャ)

	// RootSignature生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region RootParameter

	// RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)

#pragma region Texture

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	// 0から始める
	descriptorRange[0].NumDescriptors = 1;		// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで使用する数

#pragma endregion

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド 

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド 

	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

#pragma endregion

#pragma region Samplerの設定

	D3D12_STATIC_SAMPLER_DESC staticSamplers[] = {
	{
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,			// バイナリフィルタ
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,		// 0~1の範囲外をリピート
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,		// 比較しない
		.MaxLOD = D3D12_FLOAT32_MAX,						// ありったけのMipMapを使う
		.ShaderRegister = 0,								// レジスタ番号 0を使う
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,	// PixelShaderで使う
	}
	};
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

#pragma endregion


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に作成
	Microsoft::WRL::ComPtr < ID3D12RootSignature >rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region InputLayout(インプットレイアウト)

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

#pragma region BlendState(ブレンドステート)

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma endregion

#pragma region RasterizerState(ラスタライザステート)

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region ShaderをCompileする

#pragma region VertexShader

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(vertexShaderBlob != nullptr);

#pragma endregion

#pragma region PixelShader

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(pixelShaderBlob != nullptr);

#pragma endregion

#pragma endregion

#pragma region DepthStencilState

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencileResource = Texture::CreateDepthStencilTextureResource(device.Get(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>dsvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// Format。基本的にはResourceに合わせる。
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// 2dTexture
	// DSVHeapの先頭にDSVを構築する。
	device->CreateDepthStencilView(depthStencileResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

#pragma region DepthStencileStateの設定

	// DepthStencileStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効にする
	depthStencilDesc.DepthEnable = true;
	// 書き込みします。
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ表示される。
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

#pragma endregion


#pragma region PSOを生成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();													// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;														// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };		// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };		// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;																// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;														// RasterizeState

	// DSVのFormatを設定する
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形。
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定(気にしなくても良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region BallSize

	const uint16_t BallDivision = 16;
	const uint32_t BallVertexCount = 4 * BallDivision * BallDivision;

#pragma endregion

#pragma region Model

	Model model;
	model.LoadObjFile("resources", "plane.obj");
	Mesh &modelData = *model.meshList_.back();
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResourcePlane = CreateBufferResource(device.Get(), sizeof(Render::VertexData) * modelData.vertices_.size());
	// ID3D12Resource *indexResourcePlane = CreateBufferResource(device, sizeof(Render::VertexData) * modelData.vertices.size());


	D3D12_VERTEX_BUFFER_VIEW vertexBufferPlane{};
	// リソースの先頭のアドレスから使う
	vertexBufferPlane.BufferLocation = vertexResourcePlane->GetGPUVirtualAddress();
	// 使用するリソースの全体のサイズ
	vertexBufferPlane.SizeInBytes = static_cast<UINT>(sizeof(Render::VertexData) * modelData.vertices_.size());
	// 1頂点あたりのサイズ
	vertexBufferPlane.StrideInBytes = sizeof(Render::VertexData);

	// 頂点リソースにデータを書き込む
	Render::VertexData *vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourcePlane->Map(0, nullptr, reinterpret_cast<void **>(&vertexData));
	std::memcpy(vertexData, modelData.vertices_.data(), sizeof(Render::VertexData) * modelData.vertices_.size());

#pragma endregion

#pragma region ViewProjection

	Microsoft::WRL::ComPtr<ID3D12Resource>vpResource = CreateBufferResource(device.Get(), sizeof(Matrix4x4));
	Matrix4x4 *vpData = nullptr;
	vpResource->Map(0, nullptr, reinterpret_cast<void **>(&vpData));
	*vpData = Matrix4x4::Identity();

	Microsoft::WRL::ComPtr<ID3D12Resource>vpResourceUI = CreateBufferResource(device.Get(), sizeof(Matrix4x4));
	Matrix4x4 *vpDataUI = nullptr;
	vpResource->Map(0, nullptr, reinterpret_cast<void **>(&vpDataUI));
	*vpDataUI = Matrix4x4::Identity();

#pragma endregion

#pragma region VertexResourceを生成する

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device.Get(), sizeof(Render::VertexData) * 4);
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceBall = CreateBufferResource(device.Get(), sizeof(Render::VertexData) * BallVertexCount);
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource = CreateBufferResource(device.Get(), sizeof(Light::Direction));

#pragma endregion

#pragma region IndexBuffer

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceBall = CreateBufferResource(device.Get(), sizeof(uint32_t) * BallDivision * BallDivision * 6);
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device.Get(), sizeof(uint32_t) * 6);

#pragma endregion

#pragma region Material用のResource

	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズ。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(device.Get(), sizeof(Render::Material));
	// マテリアルにデータを書き込む
	Render::Material *materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData));
	materialData->color = Vector4{ 1.f,1.f,1.f,1.f };
	materialData->enableLighting = true;
	materialData->uvTransform = Matrix4x4::Identity();

#pragma region Sprite

	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズ。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device.Get(), sizeof(Render::Material));
	// マテリアルにデータを書き込む
	Render::Material *materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&materialDataSprite));
	materialDataSprite->color = Vector4{ 1.f,1.f,1.f,1.f };
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = Matrix4x4::Identity();

#pragma endregion

#pragma endregion

#pragma region VertexBufferViewを作成する

	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW lightBufferView{};
	//// リソースの先頭のアドレスから使う
	//lightBufferView.BufferLocation = lightResource->GetGPUVirtualAddress();
	//// 使用するリソースの全体サイズ
	//lightBufferView.SizeInBytes = sizeof(Light::Direction);
	//// 1頂点あたりのサイズ
	//lightBufferView.StrideInBytes = sizeof(Light::Direction);

#pragma region Sprite

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(Render::VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(Render::VertexData);


	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6u;
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

#pragma endregion

#pragma region Ball

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewBall{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewBall.BufferLocation = vertexResourceBall->GetGPUVirtualAddress();
	// 使用するリソース全体のサイズ
	vertexBufferViewBall.SizeInBytes = sizeof(Render::VertexData) * BallVertexCount;
	// 1頂点あたりのサイズ
	vertexBufferViewBall.StrideInBytes = sizeof(Render::VertexData);


	D3D12_INDEX_BUFFER_VIEW indexBufferViewBall{};

	indexBufferViewBall.BufferLocation = indexResourceBall->GetGPUVirtualAddress();
	indexBufferViewBall.SizeInBytes = sizeof(uint32_t) * 6u * BallDivision * BallDivision;
	indexBufferViewBall.Format = DXGI_FORMAT_R32_UINT;

#pragma endregion

#pragma endregion

#pragma endregion

#pragma region Transformを使ってCBufferを更新する

	// Transform変数を作る
	//Transform transform{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,10.f} };
	// カメラTransformを作る
	Transform cameraTransform{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,-5.f} };

	// Sprite用のTransform
	Transform transformSprite{ {0.6f,1.f,1.f},{0.f,0.f,0.f},{-2.f,0.f,-0.5f} };

	Transform uvTransform{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,0.f} };

	// Ball用のTransform
	Transform transformBall{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,5.f} };

#pragma endregion

#pragma region TransformationMatrix用のResourceを作る

	//// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//ID3D12Resource *wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	//// データを書き込む
	//Matrix4x4 *wvpData = nullptr;
	//// 書き込むためのアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void **>(&wvpData));
	//// 単位行列を書き込んでおく
	//*wvpData = Matrix4x4::Identity();

#pragma region Sprite

	// Sprite用のTransformationMatrixのリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(device.Get(), sizeof(Transform::TransformMatrix));
	// データを書き込む
	Transform::TransformMatrix *transformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	//transformationMatrixDataSprite->WVP = Matrix4x4::Identity();
	transformationMatrixDataSprite->World = Matrix4x4::Identity();

#pragma endregion

#pragma region Ball

	// Ball用のTransformationMatrixのリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceBall = CreateBufferResource(device.Get(), sizeof(Transform::TransformMatrix));
	// データを書き込む
	Transform::TransformMatrix *transformationMatrixDataBall = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceBall->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixDataBall));
	// 単位行列を書き込んでおく
	//transformationMatrixDataBall->WVP = Matrix4x4::Identity();
	transformationMatrixDataBall->World = Matrix4x4::Identity();

#pragma endregion

#pragma endregion

#pragma region Resourceにデータを書き込む

	// 頂点リソースにデータを書き込む
	Light::Direction *lightData = nullptr;
	// 書き込むためのアドレスを取得
	lightResource->Map(0, nullptr, reinterpret_cast<void **>(&lightData));
	lightData->color = { 1.f, 1.f, 1.f, 1.f };
	lightData->direction = Vector3{ 0.f,-1.f,0.f }.Nomalize();
	lightData->intensity = 1.f;

#pragma region Sprite

	// 頂点リソースにデータを書き込む
	Render::VertexData *vertexDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&vertexDataSprite));
	// 左下
	vertexDataSprite[0].position = { 0.f, 360.f, 0.f, 1.f };
	vertexDataSprite[0].texCoord = { 0.f,1.f };
	vertexDataSprite[0].normal = { 0.f,0.f,-1.f };
	// 左下
	vertexDataSprite[1].position = { 0.f, 0.f, 0.f, 1.f };
	vertexDataSprite[1].texCoord = { 0.f,0.f };
	vertexDataSprite[1].normal = { 0.f,0.f,-1.f };
	// 右下
	vertexDataSprite[2].position = { 640.f, 360.f, 0.f, 1.f };
	vertexDataSprite[2].texCoord = { 1.f,1.f };
	vertexDataSprite[2].normal = { 0.f,0.f,-1.f };

	//// 左下
	//vertexDataSprite[3].position = { 0.f, 0.f, 0.f, 1.f };
	//vertexDataSprite[3].texCoord = { 0.f,0.f };
	//vertexDataSprite[3].normal = { 0.f,0.f,-1.f };
	// 右上
	vertexDataSprite[3].position = { 640.f, 0.f, 0.f, 1.f };
	vertexDataSprite[3].texCoord = { 1.f,0.f };
	vertexDataSprite[3].normal = { 0.f,0.f,-1.f };
	// 右下
	/*vertexDataSprite[5].position = { 640.f, 360.f, 0.f, 1.f };
	vertexDataSprite[5].texCoord = { 1.f,1.f };
	vertexDataSprite[5].normal = { 0.f,0.f,-1.f };*/

	uint32_t *indexDataSprite =
		nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&indexDataSprite));
	indexDataSprite[0] = 0u; indexDataSprite[1] = 1u; indexDataSprite[2] = 2u;
	indexDataSprite[3] = 1u; indexDataSprite[4] = 3u; indexDataSprite[5] = 2u;

#pragma endregion

#pragma region Ball

	// 頂点リソースにデータを書き込む
	Render::VertexData *vertexDataBall = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceBall->Map(0, nullptr, reinterpret_cast<void **>(&vertexDataBall));

	CreateSphere(vertexDataBall, indexResourceBall.Get(), BallDivision);


#pragma endregion

#pragma endregion

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kWindowWidth;
	viewport.Height = WinApp::kWindowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	// シザー短形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ短形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kWindowHeight;

#pragma endregion

#pragma region Textureを読んで転送する

	// Textureを読んで転送する
	std::list<DirectX::ScratchImage>mipImagesList;
	std::list<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResourceList;
	std::list<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResoureceList;

	mipImagesList.emplace_back(Texture::Load(modelData.material_.textureFilePath));
	mipImagesList.emplace_back(Texture::Load("resources/monsterBall.png"));


	for (auto &mipImage : mipImagesList) {
		const DirectX::TexMetadata &metadata = mipImage.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D12Resource >textureResource = Texture::CreateResource(device.Get(), metadata);
		textureResourceList.push_back(textureResource);
		intermediateResoureceList.push_back(Texture::UpdateData(textureResource.Get(), mipImage, device.Get(), commandList.Get()));
	}


	//DirectX::ScratchImage mipImages = Texture::Load("resources/uvChecker.png");
	//const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
	//ID3D12Resource *textureResource = Texture::CreateResource(device, metadata);
	//ID3D12Resource *intermediateResourece = Texture::UpdateData(textureResource, mipImages, device, commandList);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからclearすること
	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	if (true) {

#pragma region コマンドをキックする

		// GPUにコマンドリストの実行を行わせる
		ID3D12CommandList *commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, commandLists);

#pragma region GPUにシグナルを送る

		// Fenceの値を更新
		fenceValue++;
		//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue->Signal(fence.Get(), fenceValue);

#pragma endregion

#pragma region Fenceの値を確認してGPUを待つ

		// Fenceの値が指定したらSignal値にたどりついているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceValue) {
			// 指定したSignalに達していないので、たどり着くまで待つようにイベントを設定する。
			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			// イベント待機
			WaitForSingleObject(fenceEvent, INFINITE);
		}

#pragma endregion

		// 次のフレーム用のコマンドリストを準備
		hr = commandAllocator->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList->Reset(commandAllocator.Get(), nullptr);
		assert(SUCCEEDED(hr));

#pragma endregion

		//for (auto &intermediateResourece : intermediateResoureceList) {
		//	//intermediateResourece->Release();
		//}
		//intermediateResoureceList.clear();
	}
#pragma endregion

#pragma region ShaderResourceViewを作る

	// metaDataを基にSRVの設定
	std::list<D3D12_SHADER_RESOURCE_VIEW_DESC> srvDescList;
	std::list<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandleGPUList;

	std::list<DirectX::ScratchImage>::iterator mipImageIterator = mipImagesList.begin();
	std::list< Microsoft::WRL::ComPtr<ID3D12Resource>>::iterator textureResourceIterator = textureResourceList.begin();
	for (uint32_t i = 0; mipImageIterator != mipImagesList.end() && textureResourceIterator != textureResourceList.end(); ++i, ++mipImageIterator, ++textureResourceIterator)
	{
		const auto &metadata = mipImageIterator->GetMetadata();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		srvDescList.push_back(srvDesc);
		// SRVを作るDescriptorHeapの場所を決める
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = DescriptorHandIe::GetCPUHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i + 1);
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = DescriptorHandIe::GetGPUHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i + 1);
		textureSrvHandleGPUList.emplace_back(textureSrvHandleGPU);
		//// 先頭はImGuiが使ってるのでその次を使う
		//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		// SRVの作成
		device->CreateShaderResourceView(textureResourceIterator->Get(), &srvDesc, textureSrvHandleCPU);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE selecteTexture = textureSrvHandleGPUList.front();
#pragma endregion


	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage()) {
			break;
		}

#pragma region ImGuiに新規フレームであると伝える

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

#pragma endregion

#pragma region ゲームの処理


		ImGui::Begin("Camera");
		// ImGui::DragFloat3("scale", &cameraTransform.scale.x, 0.1f);
		ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, Angle::Dig2Rad);
		ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.1f);
		ImGui::End();

		ImGui::Begin("UI");
		ImGui::DragFloat2("scale", &transformSprite.scale.x, 0.1f);
		ImGui::DragFloat("rotate", &transformSprite.rotate.z, Angle::Dig2Rad);
		ImGui::DragFloat2("translate", &transformSprite.translate.x, 1.f);
		ImGui::End();

		ImGui::Begin("Ball");
		if (ImGui::BeginCombo("TextureList", "texture")) {

			for (auto &texture : textureSrvHandleGPUList) {
				//bool is_selected = false;
				if (ImGui::Selectable(std::to_string(texture.ptr).c_str())) {
					selecteTexture = texture;
					break;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Image((ImTextureID)selecteTexture.ptr, { 100.f,100.f });
		ImGui::End();

		ImGui::Begin("uvTransform");
		ImGui::DragFloat2("Scale", &uvTransform.scale.x, 0.01f, -10.f, 10.f);
		ImGui::DragFloat("Rotate", &uvTransform.rotate.z, Angle::Dig2Rad);
		ImGui::DragFloat2("Transform", &uvTransform.translate.x, 0.01f, -10.f, 10.f);
		ImGui::End();

		ImGui::Begin("Light");
		ImGui::ColorEdit4("Color", &lightData->color.x);
		ImGui::DragFloat3("Direction", &lightData->direction.x, 1.f / 255, -1, 1);
		ImGui::DragFloat("Brightness ", &lightData->intensity, 0.01f, 0, 1);
		ImGui::End();
		lightData->direction = lightData->direction.Nomalize();

		// Sprite用のWorldMatrixSpriteを作る
		Matrix4x4 worldMatrixSprite = transformSprite.Affine();
		Matrix4x4 viewMatrixSprite = Matrix4x4::Identity();
		Matrix4x4 projectionMatrixSprite = Render::MakeOrthographicMatrix({ 0.f,0.f }, { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight }, 0.f, 100.f);
		Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * viewMatrixSprite * projectionMatrixSprite;
		//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
		transformationMatrixDataSprite->World = worldMatrixSprite;

		materialDataSprite->uvTransform = uvTransform.Affine();


		//transform.rotate.y += 0.03f;
		//Matrix4x4 worldMatrix = transform.Affine();
		Matrix4x4 cameraMatrix = cameraTransform.Affine();
		Matrix4x4 viewMatrix = cameraMatrix.InverseRT();
		// 透視投影行列
		Matrix4x4 projectionMatrix = Render::MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.f);
		//Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;

		//*wvpData = worldViewProjectionMatrix;


		//transformBall.rotate.y += 0.03f;
		Matrix4x4 worldMatrixBall = transformBall.Affine();

		//transformationMatrixDataBall->WVP = worldMatrixBall * viewMatrix * projectionMatrix;
		transformationMatrixDataBall->World = worldMatrixBall;

		*vpData = viewMatrix * projectionMatrix;
		*vpDataUI = viewMatrixSprite * projectionMatrixSprite;

		ImGui::ShowDemoWindow();


#pragma endregion

#pragma region ImGuiの内部コマンドを生成する

		ImGui::Render();

#pragma endregion


#pragma region コマンドを積み込んで確定させる

		// これから書き込むバックバッファのインデックスを取得
		UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrier{};
		// 今回のバリアはTransition
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。現在のバックバッファに対して行う
		barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
		// 遷移前(現在)のResourceState
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		// 遷移後のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBurrierを張る
		commandList->ResourceBarrier(1, &barrier);

#pragma endregion

		// 描画先のRTVとDSVを設定する
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
		// 指定した色で画面全体をクリアする
		float crearColor[] = { 0.1f,0.25f,0.5f,1.f }; // 青っぽい色。 RGBAの値
		commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], crearColor, 0, nullptr);

#pragma region ImGuiの描画用DescriptorHeapの設定

		// 描画用のDescriptorHeapの設定。
		ID3D12DescriptorHeap *descriptorHeaps[] = { srvDescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(1, descriptorHeaps);

#pragma endregion

#pragma region コマンドを積む

		commandList->RSSetViewports(1, &viewport);					// Viewportを設定
		commandList->RSSetScissorRects(1, &scissorRect);			// Scirssorを設定
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		commandList->SetPipelineState(graphicsPipelineState.Get());		// PSOを設定
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);	// VBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Light情報の場所を設定
		commandList->SetGraphicsRootConstantBufferView(3, lightResource->GetGPUVirtualAddress());

		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
		// Spriteの描画
		commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);	// VBVを設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(4, vpResourceUI->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, *textureSrvHandleGPUList.begin());		// TextureのSRVテーブル情報を設定
		commandList->IASetIndexBuffer(&indexBufferViewSprite);
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
		//commandList->DrawInstanced(6, 1, 0, 0);

		// Ballの描画
		commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		commandList->IASetVertexBuffers(0, 1, &vertexBufferPlane);	// VBVを設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceBall->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(4, vpResource->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, selecteTexture);
		commandList->DrawInstanced(static_cast<UINT>(modelData.vertices_.size()), 1, 0, 0);
		/*commandList->IASetIndexBuffer(&indexBufferViewBall);
		commandList->DrawIndexedInstanced(BallDivision * BallDivision * 6u, 1, 0, 0, 0);*/


#pragma endregion

#pragma region ImGuiの描画

		// 実際のCommandListにImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

#pragma endregion

#pragma region 画面状態の遷移

		// 画面に映す処理は全て終わり、画面に映すので、状態を遷移
		// 今回はRenderTargetからPresentにする
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		// TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

#pragma endregion

		// コマンドリストの内容を確定させる。すべてのコマンドを積んでからclearすること
		hr = commandList->Close();
		assert(SUCCEEDED(hr));

#pragma endregion

#pragma region コマンドをキックする

		// GPUにコマンドリストの実行を行わせる
		ID3D12CommandList *commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, commandLists);
		// GPUとOSに画面の交換を行うように通知する
		swapChain->Present(1, 0);

#pragma region GPUにシグナルを送る

		// Fenceの値を更新
		fenceValue++;
		//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue->Signal(fence.Get(), fenceValue);

#pragma endregion

#pragma region Fenceの値を確認してGPUを待つ

		// Fenceの値が指定したらSignal値にたどりついているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceValue) {
			// 指定したSignalに達していないので、たどり着くまで待つようにイベントを設定する。
			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			// イベント待機
			WaitForSingleObject(fenceEvent, INFINITE);
		}

#pragma endregion


		// 次のフレーム用のコマンドリストを準備
		hr = commandAllocator->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList->Reset(commandAllocator.Get(), nullptr);
		assert(SUCCEEDED(hr));

#pragma endregion


	}

#pragma region 各種解放

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//	vertexResourcePlane->Release();
	//
	//	indexResourceSprite->Release();
	//	indexResourceBall->Release();
	//
	//	lightResource->Release();
	//	vertexResourceSprite->Release();
	//	vertexResourceBall->Release();
	//	//vertexResource->Release();
	//	graphicsPipelineState->Release();
	//	signatureBlob->Release();
	//	if (errorBlob) {
	//		errorBlob->Release();
	//	}
	//	rootSignature->Release();
	//	pixelShaderBlob->Release();
	//	vertexShaderBlob->Release();
	//	materialResource->Release();
	//	materialResourceSprite->Release();
	//	//wvpResource->Release();
	//	transformationMatrixResourceSprite->Release();
	//	transformationMatrixResourceBall->Release();
	//	for (auto &textureResource : textureResourceList) {
	//		textureResource->Release();
	//	}
	//	depthStencileResource->Release();
	//	dsvDescriptorHeap->Release();
	//
	CloseHandle(fenceEvent);
	//	fence->Release();
	//	srvDescriptorHeap->Release();
	//	rtvDescriptorHeap->Release();
	//	swapChainResources[0]->Release();
	//	swapChainResources[1]->Release();
	//	swapChain->Release();
	//	commandList->Release();
	//	commandAllocator->Release();
	//	commandQueue->Release();
	//	device->Release();
	//	useAdapter->Release();
	//	dxgiFactory->Release();
	//#ifdef _DEBUG
	//	debugController->Release();
	//#endif // _DEBUG
	CloseWindow(winApp->GetHWND());

#pragma endregion

	CoUninitialize();

	return 0;
}