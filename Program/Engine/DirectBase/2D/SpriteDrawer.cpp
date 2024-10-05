#include "SpriteDrawer.h"

namespace SolEngine {

	void SpriteDrawer::BufferData::Generate(BufferData *ptrTarget)
	{
		// 頂点バッファを構築する
		ptrTarget->vertex_.SetVertexData(
			std::array<VertexData, 4u>
		{
			{
				{ { 0.f, 1.f, 0.f, 1.f } },	// 左下 { 0, 1 }
				{ { 0.f, 0.f, 0.f, 1.f } },	// 左上 { 0, 0 }
				{ { 1.f, 1.f, 0.f, 1.f } },	// 右下 { 1, 1 }
				{ { 1.f, 0.f, 0.f, 1.f } },	// 右上 { 1, 0 }
			}
		}
		);

		// Indexバッファを構築する
		ptrTarget->index_.SetIndexData(std::array<uint8_t, 6>{0, 1, 3, 3, 2, 0});
		///
		/// １→３
		///	↑　↓
		/// ０←２
		///

	}

	void SpriteDrawer::Init()
	{
		// バッファデータの確保
		bufferData_ = std::make_unique<BufferData>();
		// バッファデータの構築
		BufferData::Generate(bufferData_.get());

		auto *const pRootSignatureManager = ResourceObjectManager<RootSignature>::GetInstance();
		auto *const pShaderManager = ResourceObjectManager<Shader>::GetInstance();

		rootSignature_ = pRootSignatureManager->Load({ ResourceSource<RootSignature>::DefaultSampler(), {kRootParam_} });
		const PipelineState::ShaderSet shader{
			.vertex_ = pShaderManager->Load({ L"NewSprite.VS.hlsl",L"vs_6_0" }),
			.pixel_ = pShaderManager->Load({ L"NewSprite.PS.hlsl",L"ps_6_0" })
		};

		ResourceSource<PipelineState> pipelineSource;
		pipelineSource.rootSignature_ = rootSignature_;	// ルートシグネチャの指定
		pipelineSource.shaderSet_ = shader;				// シェーダの指定
		pipelineSource.inputElementDescs_ = {};			// 頂点データの指定
		pipelineSource.rasterizerDesc_ = {				// ラスタライザの指定
			.FillMode = D3D12_FILL_MODE_SOLID,	// 塗りつぶしで描画する
			.CullMode = D3D12_CULL_MODE_NONE,	// カリングは行わないものとする
		};
		pipelineSource.depthStencilDesc_ = {	// 深さデータ
			.DepthEnable = false,
		};
		pipelineSource.rtvFormats_.push_back(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);		// 通常描画

		CreatePipeline(pipelineSource);

	}

	void SpriteDrawer::CreatePipeline(ResourceSource<PipelineState> &pipeline)
	{

		// ブレンドなし
		pipeLineState_[static_cast<uint32_t>(NewSprite::BlendMode::kNone)] = ResourceCreater<PipelineState>{}.CreateObject(pipeline);

		// アルファブレンド
		// pipeLineState_

	}

}