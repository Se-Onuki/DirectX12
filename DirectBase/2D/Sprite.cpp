#include "Sprite.h"
#include "../Base/TextureManager.h"
#include "../Base/DirectXCommon.h"
#include "../../Header/Create/Create.h"
#include "../../Header/Render/Render.hpp"
#include "../Base/Shader.h"

Matrix4x4 Sprite::matProjection_{};
ID3D12Device *Sprite::device_ = nullptr;
ID3D12GraphicsCommandList *Sprite::commandList_ = nullptr;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (uint32_t)Sprite::BlendMode::kTotal> Sprite::graphicsPipelineState_ = { nullptr };
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::rootSignature_ = nullptr;

//Sprite::Sprite()
//{
//}
//
//Sprite::~Sprite()
//{
//}

void Sprite::CreatePipeLine() {


	HRESULT hr = S_FALSE;
	ID3D12Device *const device = DirectXCommon::GetInstance()->GetDevice();

#pragma region PSO(Pipeline State Object)

#pragma region RootSigneture(ルートシグネチャ)

	// RootSignature生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region RootParameter

	// RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[2] = {};

	rootParameters[(uint32_t)Sprite::RootParameter::kConstData].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[(uint32_t)Sprite::RootParameter::kConstData].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// 両方で使う
	rootParameters[(uint32_t)Sprite::RootParameter::kConstData].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド (b0が設定されているので0)

#pragma region Texture

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	// 0から始める
	descriptorRange[0].NumDescriptors = 1;		// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	rootParameters[(uint32_t)Sprite::RootParameter::kTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[(uint32_t)Sprite::RootParameter::kTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[(uint32_t)Sprite::RootParameter::kTexture].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters[(uint32_t)Sprite::RootParameter::kTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで使用する数

#pragma endregion

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
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に作成
	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
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

#pragma region Shader

	Shader vertexShader = Shader::Compile(L"Sprite.VS.hlsl", L"vs_6_0");
	Shader pixelShader = Shader::Compile(L"Object3D.PS.hlsl", L"ps_6_0");

#pragma endregion

#pragma endregion

#pragma region DepthStencilState

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
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();		// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
	graphicsPipelineStateDesc.VS = vertexShader.GetBytecode();			// VertexShader
	graphicsPipelineStateDesc.PS = pixelShader.GetBytecode();			// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;					// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;			// RasterizeState

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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[0]));
	assert(SUCCEEDED(hr));

#pragma endregion


#pragma region BlendState(ブレンドステート)

	// 全ての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[1]));
	assert(SUCCEEDED(hr));


#pragma endregion


#pragma endregion

}

void Sprite::StaticInit(ID3D12Device *device) {
	device_ = device;



}

void Sprite::Init() {
	textureHaundle_ = TextureManager::Load("white2x2.png");
	CreateBuffer();
	MapVertex();
}

void Sprite::Draw() const {
	constMap_->matWorldProjection = transform_.matWorld_ * matProjection_;


}

void Sprite::MapVertex()
{
#pragma region スプライトデータ

	constResource_->Map(0, nullptr, reinterpret_cast<void **>(&constMap_));
	constMap_->color = Vector4{ 1.f,1.f,1.f,1.f };
	constMap_->matWorldProjection = Matrix4x4::Identity();

#pragma endregion

#pragma region 頂点・インデックス

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void **>(&vertexMap_));
	// 左下
	vertexMap_[0].position = { 0.f, 360.f, 0.f, 1.f };
	vertexMap_[0].texCoord = { 0.f,1.f };
	// 左上
	vertexMap_[1].position = { 0.f, 0.f, 0.f, 1.f };
	vertexMap_[1].texCoord = { 0.f,0.f };
	// 右下
	vertexMap_[2].position = { 640.f, 360.f, 0.f, 1.f };
	vertexMap_[2].texCoord = { 1.f,1.f };

	// 右上
	vertexMap_[3].position = { 640.f, 0.f, 0.f, 1.f };
	vertexMap_[3].texCoord = { 1.f,0.f };

	// インデックス
	indexResource_->Map(0, nullptr, reinterpret_cast<void **>(&indexMap_));
	indexMap_[0] = 0u; indexMap_[1] = 1u; indexMap_[2] = 2u;
	indexMap_[3] = 1u; indexMap_[4] = 3u; indexMap_[5] = 2u;

#pragma endregion

}

void Sprite::CreateBuffer() {
	auto *const device = DirectXCommon::GetInstance()->GetDevice();

	constResource_ = CreateBufferResource(device, sizeof(ConstData) * 1u);

	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4u);
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6u);

}

void Sprite::StartDraw(ID3D12GraphicsCommandList *const commandList) {
	commandList_ = commandList;
	matProjection_ = Render::MakeOrthographicMatrix({ 0.f,0.f }, { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight }, 0.f, 100.f);
}

void Sprite::EndDraw() {
	commandList_ = nullptr;
}
