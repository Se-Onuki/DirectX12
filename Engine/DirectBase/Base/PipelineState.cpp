//#include "PipelineState.h"
//#include "DirectXCommon.h"
//
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
//	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
//	assert(SUCCEEDED(hr));
//
//
//}
//
//void PipelineState::CreatePipeline()
//{
//	//HRESULT hr = S_FALSE;
//
//	//DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
//
//
//
//	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
//	//graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();													// RootSignature
//	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;														// InputLayout
//	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };		// VertexShader
//	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };		// PixelShader
//	//graphicsPipelineStateDesc.BlendState = blendDesc[0];															// BlendState
//	//graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;														// RasterizeState
//
//	//// DSVのFormatを設定する
//	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
//	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//
//	//// 書き込むRTVの情報
//	//graphicsPipelineStateDesc.NumRenderTargets = 1;
//	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//	//// 利用するトポロジ(形状)のタイプ。三角形。
//	//graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	//// どのように画面に色を打ち込むかの設定(気にしなくても良い)
//	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
//	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//	//// 実際に生成
//	//hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
//	//assert(SUCCEEDED(hr));
//}
