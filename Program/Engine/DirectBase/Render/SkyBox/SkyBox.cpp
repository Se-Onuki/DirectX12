/// @file SkyBox.cpp
/// @brief スカイボックス
/// @author ONUKI seiya
#include "SkyBox.h"
#include "../../Base/TextureManager.h"

namespace SolEngine {

	void SkyBox::SetTexture(const std::string &file_path)
	{
		textureHandle_ = ::TextureManager::Load(file_path);
	}
	void SkyBoxRender::Init()
	{
		// バッファの構築
		CreateBuffer();

		auto *const rootSignatureManager = ResourceObjectManager<RootSignature>::GetInstance();
		auto *const shaderManager = ResourceObjectManager<Shader>::GetInstance();

		rootSignature_ = rootSignatureManager->Load({ ResourceSource<RootSignature>::DefaultSampler(),  { "t0PS,b0VS,b1VS" } });

		auto shader = PipelineState::ShaderSet{
			.vertex_ = shaderManager->Load({L"SkyBox.VS.hlsl", L"vs_6_0"}),
			.pixel_ = shaderManager->Load({L"SkyBox.PS.hlsl", L"ps_6_0"}),
		};
		auto inputElementDescs =
			D3D12_INPUT_ELEMENT_DESC{ .SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT, }
		;

		//pipeline_ = pipelineManager->Load(pipelineSource);


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = &inputElementDescs;
		inputLayoutDesc.NumElements = 1u;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_.GetResource()->Get();	// RootSignature
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
		graphicsPipelineStateDesc.RasterizerState = {
			// 三角形の中を塗りつぶす
			.FillMode = D3D12_FILL_MODE_SOLID,
			// 裏面(時計回り)を表示しない
			.CullMode = D3D12_CULL_MODE_BACK,
		};

		// DSVのFormatを設定する
		graphicsPipelineStateDesc.DepthStencilState = {
			.DepthEnable = true, // 比較は行う
			.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO,	// 書き込みは行わない
			.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL,		// 今までと同じ比較
		};

#pragma region PSOを生成する

		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトポロジ(形状)のタイプ。三角形。
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むかの設定(気にしなくても良い)
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

#pragma endregion
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};

		// 全ての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		graphicsPipelineStateDesc.BlendState = blendDesc;

		HRESULT hr = S_FALSE;

		//auto *pShaderManager_ = SolEngine::ResourceObjectManager<Shader>::GetInstance();

		shader.SetPipelineDesc(&graphicsPipelineStateDesc);

		hr = GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipeline_));

		assert(SUCCEEDED(hr));

	}

	void SkyBoxRender::Draw(const SkyBox &skyBox, const SoLib::Transform &transform, const Camera3D &camera) const
	{
		auto *const dxCommon = ::DirectXCommon::GetInstance();
		auto *const command = dxCommon->GetCommandList();
		auto *const textureManager = ::TextureManager::GetInstance();

		// RootSignatureを設定。
		command->SetGraphicsRootSignature(rootSignature_->Get());
		command->SetPipelineState(pipeline_.Get()); // PSOを設定

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い。
		command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		command->SetGraphicsRootDescriptorTable(0, textureManager->GetGpuSrvHandle(skyBox.GetTextureHandle()));
		command->SetGraphicsRootConstantBufferView(1, transform.GetGPUVirtualAddress());
		command->SetGraphicsRootConstantBufferView(2, camera.constData_.GetGPUVirtualAddress());

		command->IASetVertexBuffers(0, 1, &vertex_.GetVBView());
		command->IASetIndexBuffer(&index_.GetIBView());
		command->DrawIndexedInstanced(6u * 6u, 1, 0, 0, 0);
	}

	void SkyBoxRender::CreateBuffer()
	{

		std::array<Vector4, 4u * 2u> vertexData;

		vertexData[0] = { +1.f, +1.f, +1.f, 1.f };
		vertexData[1] = { +1.f, +1.f, -1.f, 1.f };
		vertexData[2] = { +1.f, -1.f, +1.f, 1.f };
		vertexData[3] = { +1.f, -1.f, -1.f, 1.f };

		vertexData[4] = { -1.f, +1.f, -1.f, 1.f };
		vertexData[5] = { -1.f, +1.f, +1.f, 1.f };
		vertexData[6] = { -1.f, -1.f, -1.f, 1.f };
		vertexData[7] = { -1.f, -1.f, +1.f, 1.f };

		vertex_.SetVertexData(vertexData);


		std::array<uint32_t, 6u * 6u> indexData{
			0,1,2,
			2,1,3,	// 右面

			4,5,6,
			6,5,7,	// 左面

			5,0,7,
			7,0,2,	// 前面

			1,4,3,
			3,4,6,	// 後面

			0,5,1,
			1,5,4,	// 上面

			7,2,6,
			6,2,3,	// 下面
		};

		index_.SetIndexData(indexData);
	}
}