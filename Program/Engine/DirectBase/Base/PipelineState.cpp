#include "PipelineState.h"
#include "Shader.h"
//#include "DirectXCommon.h"

namespace SolEngine {

	std::unique_ptr<PipelineState> ResourceCreater<PipelineState>::CreateObject(const ResourceSource<PipelineState> &source) const {
		// エラー検知
		HRESULT hr = S_FALSE;

		std::unique_ptr<PipelineState> result = std::make_unique<PipelineState>();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc;

#pragma region PSO(Pipeline State Object)

#pragma region InputLayout(インプットレイアウト)

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = source.inputElementDescs_.data();
		inputLayoutDesc.NumElements = static_cast<UINT>(source.inputElementDescs_.size());

#pragma endregion

#pragma region RasterizerState(ラスタライザステート)

		// RasterizerStateの設定
		const D3D12_RASTERIZER_DESC &rasterizerDesc = source.rasterizerDesc_;
		// // 裏面(時計回り)を表示しない
		// rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// // 三角形の中を塗りつぶす
		// rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;



#pragma endregion

#pragma region DepthStencileStateの設定

		// DepthStencileStateの設定
		const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc = source.depthStencilDesc_;
		// // Depthの機能を有効にする
		// depthStencilDesc.DepthEnable = true;
		// // 書き込みします。
		// depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// // 比較関数はLessEqual。つまり、近ければ表示される。
		// depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

		graphicsPipelineStateDesc.pRootSignature = source.rootSignature_->Get();		// RootSignature

		source.shaderSet_.SetPipelineDesc(&graphicsPipelineStateDesc);
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;			// RasterizeState

		// DSVのFormatを設定する
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;

#pragma region PSOを生成する

		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = source.rtvFormats_.size();
		std::copy(source.rtvFormats_.begin(), source.rtvFormats_.end(), graphicsPipelineStateDesc.RTVFormats);
		// 利用するトポロジ(形状)のタイプ。三角形。
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むかの設定(気にしなくても良い)
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;


#pragma endregion

		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};

		// データを代入
		std::copy_n(source.renderTarget_.begin(), (std::min)(source.renderTarget_.size(), 8llu), blendDesc.RenderTarget);
		// もし描画先が複数あったら
		if (source.renderTarget_.size() > 1) {
			blendDesc.IndependentBlendEnable = TRUE;	// 複数描画を有効にする
		}

		graphicsPipelineStateDesc.BlendState = blendDesc;

		// 実際に生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&result->graphicsPipelineState_));
		assert(SUCCEEDED(hr));


#pragma endregion

		return std::move(result);

	}
}

//void PipelineState::SetRootSignature()
//{
//	HRESULT hr = S_FALSE;
//
//	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
//
//
//
//	// シリアライズしてバイナリにする
//	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
//	hr = D3D12SerializeRootSignature(&descriptionRootSignature_, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
//	if (FAILED(hr)) {
//		DirectXCommon::Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
//		assert(false);
//	}
//	// バイナリを元に作成
//	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
//	assert(SUCCEEDED(hr));
//
//
//}
//
//void PipelineState::CreatePipeline()
//{
//	HRESULT hr = S_FALSE;
//
//	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
//
//#pragma region InputLayout(インプットレイアウト)
//
//	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
//	inputLayoutDesc.pInputElementDescs = inputElementDescs_.data();
//	inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs_.size());
//
//#pragma endregion
//
//}
//
//void PipelineState::Create(const RootSignature *rootSignature, const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc) {
//
//	HRESULT hr = S_FALSE;
//	//ID3D12Device *const device = DirectXCommon::GetInstance()->GetDevice();
//
//	rootSignature_ = rootSignature;
//
//#pragma region PSO(Pipeline State Object)
//
//#pragma region InputLayout(インプットレイアウト)
//
//	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
//	inputLayoutDesc.pInputElementDescs = inputElementDescs_.data();
//	inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs_.size());
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
//	graphicsPipelineStateDesc_.pRootSignature = rootSignature_->Get();		// RootSignature
//
//	shaderSet_.SetPipelineDesc(&graphicsPipelineStateDesc_);
//	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc;			// InputLayout
//	graphicsPipelineStateDesc_.RasterizerState = rasterizerDesc;			// RasterizeState
//
//	// DSVのFormatを設定する
//	graphicsPipelineStateDesc_.DepthStencilState = depthStencilDesc;
//
//#pragma region PSOを生成する
//
//	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//
//	// 書き込むRTVの情報
//	graphicsPipelineStateDesc_.NumRenderTargets = 1;
//	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//	// 利用するトポロジ(形状)のタイプ。三角形。
//	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	// どのように画面に色を打ち込むかの設定(気にしなくても良い)
//	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
//	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//
//
//#pragma endregion
//
//	// BlendStateの設定
//	D3D12_BLEND_DESC blendDesc{};
//
//	// 全ての色要素を書き込む
//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	graphicsPipelineStateDesc_.BlendState = blendDesc;
//
//	// 実際に生成
//	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
//	assert(SUCCEEDED(hr));
//
//
//#pragma endregion
//
//}
//
//void PipelineState::SetShader(const ShaderSet &shaderSet) {
//	shaderSet_ = shaderSet;
//}

void PipelineState::ShaderSet::SetPipelineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC *const pipelineDesc) const {
	if (vertex_) { pipelineDesc->VS = vertex_->GetBytecode(); }
	if (pixel_) { pipelineDesc->PS = pixel_->GetBytecode(); }
	if (domain_) { pipelineDesc->DS = domain_->GetBytecode(); }
	if (hull_) { pipelineDesc->HS = hull_->GetBytecode(); }
	if (geometry_) { pipelineDesc->GS = geometry_->GetBytecode(); }
}
