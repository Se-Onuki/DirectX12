#include "Sprite.h"
#include <array>

#include "../Base/TextureManager.h"
#include "../Base/DirectXCommon.h"
#include "../Create/Create.h"
#include "../Render/Render.h"
#include "../Base/Shader.h"
DirectResourceLeakChecker Sprite::leakChecker{};

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
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	// カリングを無効化
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region ShaderをCompileする

	Shader vertexShader = Shader::Compile(L"Sprite.VS.hlsl", L"vs_6_0");
	Shader pixelShader = Shader::Compile(L"Sprite.PS.hlsl", L"ps_6_0");

#pragma endregion

#pragma region DepthStencilState

#pragma region DepthStencileStateの設定

	// DepthStencileStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効にする
	depthStencilDesc.DepthEnable = true;
	// 書き込みします。
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はalways。必ず書き換える
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

#pragma endregion

#pragma endregion


#pragma region PSOを生成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();	// RootSignature
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[(uint32_t)BlendMode::kNone]));
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region αブレンド 


	blendDesc.RenderTarget[0].BlendEnable = true;						// ブレンドモードを有効にするか
	// RGB値の操作
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;				// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// レンダーターゲットからの受け取り方
	// α値の操作
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;		// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;		// レンダーターゲットからの受け取り方

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[(uint32_t)BlendMode::kNormal]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region 加算合成 Dest * 1 + Src * Src.Alpha

	blendDesc.RenderTarget[0].BlendEnable = true;						// ブレンドモードを有効にするか
	// RGB値の操作
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;				// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;				// レンダーターゲットからの受け取り方
	// α値の操作
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;		// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;		// レンダーターゲットからの受け取り方

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[(uint32_t)BlendMode::kAdd]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region 減算合成 Dest * 1 - Src * Src.Alpha

	blendDesc.RenderTarget[0].BlendEnable = true;						// ブレンドモードを有効にするか
	// RGB値の操作
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;		// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;				// レンダーターゲットからの受け取り方
	// α値の操作
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;		// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;		// レンダーターゲットからの受け取り方

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[(uint32_t)BlendMode::kSubtract]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region 乗算合成 Src * 0 + Dest * Src

	blendDesc.RenderTarget[0].BlendEnable = true;						// ブレンドモードを有効にするか
	// RGB値の操作
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;				// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;				// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;		// レンダーターゲットからの受け取り方
	// α値の操作
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;		// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;		// レンダーターゲットからの受け取り方

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[(uint32_t)BlendMode::kMultily]));
	assert(SUCCEEDED(hr));


#pragma endregion

#pragma region スクリーン合成 Src * (1 - Dest) + Dest * 1

	blendDesc.RenderTarget[0].BlendEnable = true;						// ブレンドモードを有効にするか
	// RGB値の操作
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;				// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;				// レンダーターゲットからの受け取り方
	// α値の操作
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;		// レンダーターゲットへの論理操作
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;			// シェーダから得たデータの受け取り方
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;		// レンダーターゲットからの受け取り方

	graphicsPipelineStateDesc.BlendState = blendDesc;

	// 実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[(uint32_t)BlendMode::kScreen]));
	assert(SUCCEEDED(hr));


#pragma endregion


#pragma endregion

}

void Sprite::StaticInit() {

	device_ = DirectXCommon::GetInstance()->GetDevice();

	CreatePipeLine();

}

void Sprite::Init(const std::string &textureName) {
	Init(TextureManager::Load(textureName));
}

void Sprite::Init(const uint32_t &textureHaundle) {
	CreateBuffer();
	MapVertex();
	CalcBuffer();

	SetTextureHaundle(textureHaundle);
	transform_.matWorld_ = Matrix4x4::Identity();
}

void Sprite::Draw() const {
	if (isVisible_) {	// 不可視であれば飛ばす
		constData_->matWorldProjection = transform_.matWorld_ * matProjection_;

		// マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView((uint32_t)RootParameter::kConstData, constData_.GetGPUVirtualAddress());
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable((uint32_t)RootParameter::kTexture, textureHaundle_);
		// Spriteの描画
		commandList_->IASetVertexBuffers(0, 1, &vertexData_.GetVBView());	// VBVを設定
		commandList_->IASetIndexBuffer(&vertexData_.GetIBView());
		commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Sprite::MapVertex()
{
#pragma region スプライトデータ

	constData_->color = Vector4{ 1.f,1.f,1.f,1.f };
	constData_->matWorldProjection = Matrix4x4::Identity();

#pragma endregion

#pragma region 頂点・インデックス

	//// 書き込むためのアドレスを取得
	//vertexResource_->Map(0, nullptr, reinterpret_cast<void **>(&vertexMap_));

	auto &vertexArray = vertexData_.GetVertexData();

	// 左下
	vertexArray[(uint32_t)VertexNumer::LDown].position = { 0.f, 1.f, 0.f, 1.f };
	vertexArray[(uint32_t)VertexNumer::LDown].texCoord = { 0.f,1.f };
	// 左上
	vertexArray[(uint32_t)VertexNumer::LTop].position = { 0.f, 0.f, 0.f, 1.f };
	vertexArray[(uint32_t)VertexNumer::LTop].texCoord = { 0.f,0.f };
	// 右下
	vertexArray[(uint32_t)VertexNumer::RDown].position = { 1.f, 1.f, 0.f, 1.f };
	vertexArray[(uint32_t)VertexNumer::RDown].texCoord = { 1.f,1.f };

	// 右上
	vertexArray[(uint32_t)VertexNumer::RTop].position = { 1.f, 0.f, 0.f, 1.f };
	vertexArray[(uint32_t)VertexNumer::RTop].texCoord = { 1.f,0.f };

#pragma endregion

}

void Sprite::CreateBuffer() {

	vertexData_.SetVertexData(std::array<VertexData, 4u>{ VertexData{}, VertexData{}, VertexData{}, VertexData{} });
	vertexData_.SetIndexData(std::array<uint32_t, 6u>{ 0u, 1u, 2u, 1u, 3u, 2u });
	//auto *const device = DirectXCommon::GetInstance()->GetDevice();

	//vertexData_ = CreateBufferResource(device, sizeof(VertexData) * 4u);
	//indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6u);


	//// 頂点バッファビューを作成する
	//// リソースの先頭のアドレスから使う
	//vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点3つ分のサイズ
	//vbView_.SizeInBytes = sizeof(VertexData) * 4;
	//// 1頂点あたりのサイズ
	//vbView_.StrideInBytes = sizeof(VertexData);


	//// インデックスview
	//ibView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//ibView_.SizeInBytes = sizeof(uint32_t) * 6u;
	//ibView_.Format = DXGI_FORMAT_R32_UINT;

}

void Sprite::StartDraw(ID3D12GraphicsCommandList *const commandList) {
	assert(!commandList_ && "EndDrawが呼び出されていません");
	commandList_ = commandList;

	// RootSignatureを設定。
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_[(uint32_t)BlendMode::kNormal].Get());		// PSOを設定

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	matProjection_ = Render::MakeOrthographicMatrix({ 0.f,0.f }, { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight }, 0.f, 100.f);
}

void Sprite::EndDraw() {
	commandList_ = nullptr;
}

void Sprite::SetBlendMode(BlendMode blendMode)
{
	commandList_->SetPipelineState(graphicsPipelineState_[(uint32_t)blendMode].Get());		// PSOを設定

}

void Sprite::ImGuiWidget() {
	if (transform_.ImGuiWidget2D()) {
		transform_.CalcMatrix();
	}
	ImGui::ColorEdit4("Color", &constData_->color.x);
	static char filePath[32];
	ImGui::InputText("filePath", filePath, 32u);
	if (ImGui::Button("Load")) {
		SetTextureHaundle(TextureManager::Load(filePath));
	}
	if (ImGui::Button("Set 'white2x2.png'")) {
		SetTextureHaundle(TextureManager::Load("white2x2.png"));
	}
	ImGui::SameLine();
	if (ImGui::Button("Set 'uvChecker.png'")) {
		SetTextureHaundle(TextureManager::Load("uvChecker.png"));
	}

	bool uvMoveing = false;
	uvMoveing |= ImGui::DragFloat2("Pivot", &pivot_.x, 0.05f, 0.f, 1.f);

	uvMoveing |= ImGui::DragFloat2("uvOrigin", &uv_.first.x, 1.f, 0.f, 1024.f);
	uvMoveing |= ImGui::DragFloat2("uvDiff", &uv_.second.x, 1.f, 0.f, 1024.f);
	if (uvMoveing) {
		CalcBuffer();
	}

}


void Sprite::SetScale(const Vector2 &scale)
{
	transform_.scale = { scale.x,scale.y,1.f };
	transform_.CalcMatrix();
}
void Sprite::SetRotate(const float angle)
{
	transform_.rotate = { 0.f,0.f,angle };
	transform_.CalcMatrix();
}

void Sprite::SetPosition(const Vector2 &position)
{
	transform_.translate = { position.x,position.y,0.f };
	transform_.CalcMatrix();
}

std::unique_ptr<Sprite> Sprite::Create() {
	auto sprite = std::make_unique<Sprite>();
	sprite->Init();
	return std::move(sprite);
}

void Sprite::CalcBuffer() {
	auto &vertexArray = vertexData_.GetVertexData();
#pragma region 座標データ

	vertexArray[(uint32_t)VertexNumer::LDown].position = { -pivot_.x, 1.f - pivot_.y, 0.f, 1.f };	// 左下 { 0, 1 }
	vertexArray[(uint32_t)VertexNumer::LTop].position = { -pivot_.x,  -pivot_.y, 0.f, 1.f };	// 左上 { 0, 0 }
	vertexArray[(uint32_t)VertexNumer::RDown].position = { 1.f - pivot_.x, 1.f - pivot_.y, 0.f, 1.f };	// 右下 { 1, 1 }
	vertexArray[(uint32_t)VertexNumer::RTop].position = { 1.f - pivot_.x, -pivot_.y, 0.f, 1.f };	// 右上 { 1, 0 }

#pragma endregion

#pragma region UV情報

	Vector2 texOrigin = { uv_.first.x / resourceDesc.Width,uv_.first.y / resourceDesc.Height };
	Vector2 texDiff = { (uv_.first.x + uv_.second.x) / resourceDesc.Width, (uv_.first.y + uv_.second.y) / resourceDesc.Height };

	vertexArray[(uint32_t)VertexNumer::LDown].texCoord = { texOrigin.x,texDiff.y };	// 左下 { 0, 1 }
	vertexArray[(uint32_t)VertexNumer::LTop].texCoord = { texOrigin.x,texOrigin.y };	// 左上 { 0, 0 }
	vertexArray[(uint32_t)VertexNumer::RDown].texCoord = { texDiff.x,texDiff.y };	// 右下 { 1, 1 }
	vertexArray[(uint32_t)VertexNumer::RTop].texCoord = { texDiff.x,texOrigin.y };	// 右上 { 1, 0 }

#pragma endregion

	transform_.CalcMatrix();
}

std::unique_ptr<Sprite> Sprite::Create(const uint32_t textureHaundle)
{
	auto sprite = std::make_unique<Sprite>();
	sprite->Init(textureHaundle);
	return std::move(sprite);
}

std::unique_ptr<Sprite> Sprite::Create(const uint32_t textureHaundle, const Vector2 &position, const Vector2 &scale)
{
	auto sprite = Create(textureHaundle);
	sprite->transform_.translate = { position.x,position.y,0.f };
	sprite->transform_.scale = { scale.x,scale.y,1.f };
	sprite->transform_.CalcMatrix();
	return std::move(sprite);
}

void Sprite::SetColor(const Vector4 &color)
{
	constData_->color = color;
}

const Vector4 &Sprite::GetColor() const
{
	return constData_->color;
}


void Sprite::SetTexOrigin(const Vector2 &texOrigin) {
	uv_.first = texOrigin;
	CalcBuffer();
}

void Sprite::SetTexDiff(const Vector2 &texDiff) {
	uv_.second = texDiff;
	CalcBuffer();
}

void Sprite::SetPivot(const Vector2 &pivot) {
	pivot_ = pivot;
	CalcBuffer();
}
