#include "DirectBase/Base/LeakChecker.h"

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
#include "DirectBase/3D/ViewProjection/ViewProjection.h"
#include "DirectBase/Base/DirectXCommon.h"
#include "DirectBase/Base/TextureManager.h"
#include "DirectBase/Base/Shader.h"
#include "DirectBase/Base/ImGuiManager.h"

//template <typename T>using  Microsoft::WRL::ComPtr = Microsoft::WRL:: Microsoft::WRL::ComPtr<T>;
//using namespace Microsoft::WRL;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	WinApp::StaticInit();

	WinApp *const winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("DirectXGame");

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	dxCommon->Init(winApp);

	ID3D12GraphicsCommandList *const commandList_ = dxCommon->commandList_.Get();

	TextureManager *const textureManager = TextureManager::GetInstance();

	textureManager->Init(dxCommon->GetDevice(), commandList_);
	uint32_t uvTex =
		TextureManager::Load("white2x2.png");

	//HRESULT hr;

#pragma region DescriptorSize

#pragma endregion

#pragma region DescriptorHeap

	ID3D12DescriptorHeap *const srvDescriptorHeap = textureManager->GetSRVHeap();


#pragma endregion


	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む


#pragma region ImGuiの初期化

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(dxCommon->GetDevice(),
		(int32_t)dxCommon->backBuffers_.size(),
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

#pragma endregion

	// FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	Shader::StaticInit();
	Model::StaticInit();


	//#pragma region PSO(Pipeline State Object)
	//
	//#pragma region RootSigneture(ルートシグネチャ)
	//
	//	// RootSignature生成
	//	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//	descriptionRootSignature.Flags =
	//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//
	//#pragma region RootParameter
	//
	//	// RootParameter作成
	//	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	//
	//	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	//	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	//	rootParameters[(uint32_t)Model::RootParameter::kWorldTransform].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)
	//
	//	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	//	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	//	rootParameters[(uint32_t)Model::RootParameter::kViewProjection].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド 
	//
	//
	//	rootParameters[(uint32_t)Model::RootParameter::kMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	//	rootParameters[(uint32_t)Model::RootParameter::kMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	//	rootParameters[(uint32_t)Model::RootParameter::kMaterial].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)
	//
	//	rootParameters[(uint32_t)Model::RootParameter::kLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	//	rootParameters[(uint32_t)Model::RootParameter::kLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	//	rootParameters[(uint32_t)Model::RootParameter::kLight].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド 
	//
	//#pragma region Texture
	//
	//	// DescriptorRangeの設定
	//	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//	descriptorRange[0].BaseShaderRegister = 0;	// 0から始める
	//	descriptorRange[0].NumDescriptors = 1;		// 数は1つ
	//	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	//	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算
	//
	//	rootParameters[(uint32_t)Model::RootParameter::kTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	//	rootParameters[(uint32_t)Model::RootParameter::kTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	//	rootParameters[(uint32_t)Model::RootParameter::kTexture].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	//	rootParameters[(uint32_t)Model::RootParameter::kTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで使用する数
	//
	//#pragma endregion
	//
	//	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	//	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ
	//
	//#pragma endregion
	//
	//#pragma region Samplerの設定
	//
	//	D3D12_STATIC_SAMPLER_DESC staticSamplers[] = {
	//	{
	//		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,			// バイナリフィルタ
	//		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,		// 0~1の範囲外をリピート
	//		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	//		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	//		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,		// 比較しない
	//		.MaxLOD = D3D12_FLOAT32_MAX,						// ありったけのMipMapを使う
	//		.ShaderRegister = 0,								// レジスタ番号 0を使う
	//		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,	// PixelShaderで使う
	//	}
	//	};
	//	descriptionRootSignature.pStaticSamplers = staticSamplers;
	//	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	//
	//#pragma endregion
	//
	//	// シリアライズしてバイナリにする
	//	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	//	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	//	if (FAILED(hr)) {
	//		DirectXCommon::Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
	//		assert(false);
	//	}
	//	// バイナリを元に作成
	//	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	//	assert(SUCCEEDED(hr));
	//
	//#pragma endregion
	//
	//#pragma region InputLayout(インプットレイアウト)
	//
	//	// InputLayout
	//	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//
	//	inputElementDescs[0].SemanticName = "POSITION";
	//	inputElementDescs[0].SemanticIndex = 0;
	//	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//
	//	inputElementDescs[1].SemanticName = "TEXCOORD";
	//	inputElementDescs[1].SemanticIndex = 0;
	//	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//
	//	inputElementDescs[2].SemanticName = "NORMAL";
	//	inputElementDescs[2].SemanticIndex = 0;
	//	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//
	//	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	//	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	//	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	//
	//#pragma endregion
	//
	//#pragma region BlendState(ブレンドステート)
	//
	//	// BlendStateの設定
	//	D3D12_BLEND_DESC blendDesc[2]{};
	//	// 全ての色要素を書き込む
	//	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//
	//	// 全ての色要素を書き込む
	//	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//	blendDesc[1].RenderTarget[0].BlendEnable = true;
	//	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//
	//	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//
	//#pragma endregion
	//
	//#pragma region RasterizerState(ラスタライザステート)
	//
	//	// RasterizerStateの設定
	//	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//	// 裏面(時計回り)を表示しない
	//	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//	// 三角形の中を塗りつぶす
	//	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//
	//#pragma endregion
	//
	//#pragma region ShaderをCompileする
	//
	//#pragma region VertexShader
	//
	//	Shader vertexShader = Shader::Compile(L"Object3D.VS.hlsl", L"vs_6_0");
	//
	//#pragma endregion
	//
	//#pragma region PixelShader
	//
	//	Shader pixelShader = Shader::Compile(L"Object3D.PS.hlsl", L"ps_6_0");
	//
	//#pragma endregion
	//
	//#pragma endregion
	//
	//#pragma region DepthStencilState
	//
	//	//Microsoft::WRL::ComPtr<ID3D12Resource> depthStencileResource = Texture::CreateDepthStencilTextureResource(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);
	//
	//	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>dsvDescriptorHeap = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//
	//	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// Format。基本的にはResourceに合わせる。
	//	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// 2dTexture
	//	//// DSVHeapの先頭にDSVを構築する。
	//	//dxCommon->GetDevice()->CreateDepthStencilView(depthStencileResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	//
	//#pragma region DepthStencileStateの設定
	//
	//	// DepthStencileStateの設定
	//	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//	// Depthの機能を有効にする
	//	depthStencilDesc.DepthEnable = true;
	//	// 書き込みします。
	//	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//	// 比較関数はLessEqual。つまり、近ければ表示される。
	//	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//
	//#pragma endregion
	//
	//#pragma endregion
	//
	//
	//#pragma region PSOを生成する
	//
	//	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();		// RootSignature
	//	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
	//	graphicsPipelineStateDesc.VS = vertexShader.GetBytecode();			// VertexShader
	//	graphicsPipelineStateDesc.PS = pixelShader.GetBytecode();			// PixelShader
	//	graphicsPipelineStateDesc.BlendState = blendDesc[0];				// BlendState
	//	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;			// RasterizeState
	//
	//	// DSVのFormatを設定する
	//	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//
	//	// 書き込むRTVの情報
	//	graphicsPipelineStateDesc.NumRenderTargets = 1;
	//	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//	// 利用するトポロジ(形状)のタイプ。三角形。
	//	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	// どのように画面に色を打ち込むかの設定(気にしなくても良い)
	//	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//	// 実際に生成
	//	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState[2] = { nullptr };
	//	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState[0]));
	//	assert(SUCCEEDED(hr));
	//
	//#pragma endregion
	//
	//#pragma region PSOを生成する
	//
	//	graphicsPipelineStateDesc.BlendState = blendDesc[1];
	//	// 実際に生成
	//	//Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState = nullptr;
	//	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState[1]));
	//	assert(SUCCEEDED(hr));
	//
	//#pragma endregion
	//
	//
	//#pragma endregion

#pragma region Model

	std::unique_ptr<Model> model;

	model.reset(Model::LoadObjFile("", "multiMaterial.obj"));
	//model[1].reset(Model::LoadObjFile("", "bunny.obj"));

	//const std::unique_ptr<Model>const model2{ Model::LoadObjFile("", "bunny.obj") };
	//model.LoadObjFile("", "plane.obj");


#pragma endregion

#pragma region ViewProjection

	Microsoft::WRL::ComPtr<ID3D12Resource>vpResourceUI = CreateBufferResource(dxCommon->GetDevice(), sizeof(ViewProjection::ViewProjectionMatrix));
	ViewProjection::ViewProjectionMatrix *vpDataUI = nullptr;
	vpResourceUI->Map(0, nullptr, reinterpret_cast<void **>(&vpDataUI));
	vpDataUI->view = Matrix4x4::Identity();
	vpDataUI->projection = Matrix4x4::Identity();

	ViewProjection viewProjection;
	viewProjection.Init();

#pragma endregion

#pragma region VertexResourceを生成する

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(dxCommon->GetDevice(), sizeof(Mesh::VertexData) * 4);
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceBall = CreateBufferResource(dxCommon->GetDevice(), sizeof(Mesh::VertexData) * BallVertexCount);

	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Light::Direction));

#pragma endregion

#pragma region IndexBuffer

	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceBall = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t) * BallDivision * BallDivision * 6);
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t) * 6);

#pragma endregion

#pragma region Material用のResource

	//// マテリアル用のリソースを作る。今回はcolor1つ分のサイズ。
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Material::MaterialData));
	//// マテリアルにデータを書き込む
	//Material::MaterialData *materialData = nullptr;
	//// 書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData));
	//materialData->color = Vector4{ 1.f,1.f,1.f,1.f };
	//materialData->enableLighting = true;
	//materialData->uvTransform = Matrix4x4::Identity();

#pragma region Sprite

	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズ。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(dxCommon->GetDevice(), sizeof(Material::MaterialData));
	// マテリアルにデータを書き込む
	Material::MaterialData *materialDataSprite = nullptr;
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
	vertexBufferViewSprite.SizeInBytes = sizeof(Mesh::VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(Mesh::VertexData);


	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6u;
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

#pragma endregion

#pragma region Ball

	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewBall{};
	//// リソースの先頭のアドレスから使う
	//vertexBufferViewBall.BufferLocation = vertexResourceBall->GetGPUVirtualAddress();
	//// 使用するリソース全体のサイズ
	//vertexBufferViewBall.SizeInBytes = sizeof(Mesh::VertexData) * BallVertexCount;
	//// 1頂点あたりのサイズ
	//vertexBufferViewBall.StrideInBytes = sizeof(Mesh::VertexData);


	//D3D12_INDEX_BUFFER_VIEW indexBufferViewBall{};

	//indexBufferViewBall.BufferLocation = indexResourceBall->GetGPUVirtualAddress();
	//indexBufferViewBall.SizeInBytes = sizeof(uint32_t) * 6u * BallDivision * BallDivision;
	//indexBufferViewBall.Format = DXGI_FORMAT_R32_UINT;

#pragma endregion

#pragma endregion

#pragma region Transformを使ってCBufferを更新する

	// Sprite用のTransform
	Transform transformSprite{ {0.6f,1.f,1.f},{0.f,0.f,0.f},{-2.f,0.f,-0.5f} };

	Transform uvTransform{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,0.f} };

	// Ball用のTransform
	Transform transformBall{ {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,5.f} };
	transformBall.InitResource();

#pragma endregion

#pragma region TransformationMatrix用のResourceを作る

#pragma region Sprite

	// Sprite用のTransformationMatrixのリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(dxCommon->GetDevice(), sizeof(Transform::TransformMatrix));
	// データを書き込む
	Transform::TransformMatrix *transformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	//transformationMatrixDataSprite->WVP = Matrix4x4::Identity();
	transformationMatrixDataSprite->World = Matrix4x4::Identity();

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
	Mesh::VertexData *vertexDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&vertexDataSprite));
	// 左下
	vertexDataSprite[0].position = { 0.f, 360.f, 0.f, 1.f };
	vertexDataSprite[0].texCoord = { 0.f,1.f };
	vertexDataSprite[0].normal = { 0.f,0.f,-1.f };
	// 左上
	vertexDataSprite[1].position = { 0.f, 0.f, 0.f, 1.f };
	vertexDataSprite[1].texCoord = { 0.f,0.f };
	vertexDataSprite[1].normal = { 0.f,0.f,-1.f };
	// 右下
	vertexDataSprite[2].position = { 640.f, 360.f, 0.f, 1.f };
	vertexDataSprite[2].texCoord = { 1.f,1.f };
	vertexDataSprite[2].normal = { 0.f,0.f,-1.f };

	// 右上
	vertexDataSprite[3].position = { 640.f, 0.f, 0.f, 1.f };
	vertexDataSprite[3].texCoord = { 1.f,0.f };
	vertexDataSprite[3].normal = { 0.f,0.f,-1.f };

	uint32_t *indexDataSprite =
		nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&indexDataSprite));
	indexDataSprite[0] = 0u; indexDataSprite[1] = 1u; indexDataSprite[2] = 2u;
	indexDataSprite[3] = 1u; indexDataSprite[4] = 3u; indexDataSprite[5] = 2u;

#pragma endregion

#pragma region Ball

	//// 頂点リソースにデータを書き込む
	//Mesh::VertexData *vertexDataBall = nullptr;
	//// 書き込むためのアドレスを取得
	//vertexResourceBall->Map(0, nullptr, reinterpret_cast<void **>(&vertexDataBall));

	//Mesh::CreateSphere(vertexDataBall, indexResourceBall.Get(), BallDivision);


#pragma endregion

#pragma endregion

		// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage()) break;

#pragma region ImGuiに新規フレームであると伝える
		ImGuiManager::StartFlame();
#pragma endregion

#pragma region ゲームの処理

		ImGui::Begin("Camera");
		ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, Angle::Dig2Rad);
		ImGui::DragFloat3("translate", &viewProjection.translation_.x, 0.1f);
		ImGui::End();

		ImGui::Begin("OBJ");
		transformBall.ImGuiWidget();
		ImGui::End();

		ImGui::Begin("UI");
		ImGui::DragFloat2("scale", &transformSprite.scale.x, 0.1f);
		ImGui::DragFloat("rotate", &transformSprite.rotate.z, Angle::Dig2Rad);
		ImGui::DragFloat2("translate", &transformSprite.translate.x, 1.f);
		ImGui::End();

		ImGui::Begin("Material");
		ImGui::ColorEdit4("Color", &materialDataSprite->color.x);
		ImGui::Checkbox("Lighting", (bool *)&materialDataSprite->enableLighting);
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
		//Matrix4x4 wvpnMatrixSprite = worldMatrixSprite * viewMatrixSprite * projectionMatrixSprite;
		//transformationMatrixDataSprite->WVP = wvpnMatrixSprite;
		transformationMatrixDataSprite->World = worldMatrixSprite;

		materialDataSprite->uvTransform = uvTransform.Affine();


		// 透視投影行列
		//Matrix4x4 projectionMatrix = Render::MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.f);

		//Matrix4x4 worldMatrixBall = transformBall.Affine();
		transformBall.UpdateMatrix();

		//transformationMatrixDataBall->World = worldMatrixBall;


		viewProjection.UpdateMatrix();

		vpDataUI->view = viewMatrixSprite;
		vpDataUI->projection = projectionMatrixSprite;

		ImGui::Begin("model");
		model->ImGuiWidget();
		ImGui::End();

		ImGui::ShowDemoWindow();

#pragma endregion

#pragma region ImGuiの内部コマンドを生成する

		ImGui::Render();

#pragma endregion


#pragma region コマンドを積み込んで確定させる

		dxCommon->StartDraw();

		textureManager->StartDraw();

		Model::StartDraw(commandList_);

#pragma region コマンドを積む

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
		commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Light情報の場所を設定
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kLight, lightResource->GetGPUVirtualAddress());

		// マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, materialResourceSprite->GetGPUVirtualAddress());
		// Spriteの描画
		commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);	// VBVを設定
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transformationMatrixResourceSprite->GetGPUVirtualAddress());		// wvp用のCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, vpResourceUI->GetGPUVirtualAddress());
		textureManager->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kTexture, uvTex);
		commandList_->IASetIndexBuffer(&indexBufferViewSprite);
		commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

		// Ballの描画
		model->Draw(transformBall, viewProjection);

		Model::EndDraw();

#pragma endregion

#pragma region ImGuiの描画

		// 実際のCommandListにImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_);

#pragma endregion
		dxCommon->EndDraw();

#pragma endregion

	}

#pragma region 各種解放

	ImGuiManager::Finalize();

	dxCommon->Finalize();

	winApp->Finalize();

#pragma endregion

	return 0;
}