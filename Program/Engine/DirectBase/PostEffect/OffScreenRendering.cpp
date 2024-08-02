#include "OffScreenRendering.h"
#include "../Base/PipelineState.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "../../../Utils/Math/Transform.h"
#include "../../../externals/DirectXTex/d3dx12.h"

namespace PostEffect {

	void OffScreenRenderer::Init()
	{
		WinApp *winApp = WinApp::GetInstance();
		auto device = GetDevice();
		auto srvDescHeap = GetDescHeapCbvSrvUav();

#ifdef _DEBUG

		//assert(device and "Deviceがnullptrです");

#endif // _DEBUG

		// RTVの作成
		rtvDescHeap_ = std::make_unique<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>>(device, 1, false);

		//
		rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 出力結果をSRGBに変換して書き込む
		rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む

		// 描画先のテクスチャ
		renderTargetTexture_ = OffScreenRenderer::CreateRenderTextrueResource(device, winApp->kWindowWidth, winApp->kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);


		device->CreateRenderTargetView(renderTargetTexture_.Get(), &rtvDesc_, rtvDescHeap_->GetHandle(0, 0).cpuHandle_);

		// SRVの作成
		D3D12_SHADER_RESOURCE_VIEW_DESC renderTexturSrvDesc_{};
		renderTexturSrvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTexturSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		renderTexturSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		renderTexturSrvDesc_.Texture2D.MipLevels = 1u;

		srvHeapRange_ = srvDescHeap->RequestHeapAllocation(1);

		device->CreateShaderResourceView(renderTargetTexture_.Get(), &renderTexturSrvDesc_, srvHeapRange_.GetHandle(0).cpuHandle_);

	}

	OffScreenRenderer::ComPtr<ID3D12Resource> OffScreenRenderer::CreateRenderTextrueResource(ID3D12Device *device, uint32_t width, uint32_t height, DXGI_FORMAT format, const SoLib::Color::RGB4 &clearColor)
	{
		// 1. metadataを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width;                                   // textureの幅
		resourceDesc.Height = height;                                 // textureの高さ
		resourceDesc.MipLevels = 1;                                   // mipMapの数
		resourceDesc.DepthOrArraySize = 1;                            // 奥行き or 配列Textureの配列数
		resourceDesc.Format = format;                                 // TextureのFormat
		resourceDesc.SampleDesc.Count = 1;                            // サンプリングカウント
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  // Textureの次元数。普段使っているのは2次元。
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTargetとして利用できるようにする

		// 2. 利用するHeapの設定。
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作成

		// クリアする色の設定
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		for (int i = 0; i < 4; i++) {
			clearValue.Color[i] = clearColor[i];
		}

		// 3. Resourceを生成する
		ComPtr<ID3D12Resource> resource = nullptr;
		HRESULT hr = S_FALSE;
		hr = device->CreateCommittedResource(
			&heapProperties,                    // Heapの設定
			D3D12_HEAP_FLAG_NONE,               // Heapの特殊な設定。特になし。
			&resourceDesc,                      // Resourceの設定
			D3D12_RESOURCE_STATE_RENDER_TARGET, // これから描画することを前提としたTextureなのでRenderTargetとして使うことから始める
			&clearValue,                        // Clear最適値｡ ClearRenderTargetのClearがこの値で行われる
			IID_PPV_ARGS(&resource));           // 作成するResourceへのポインタ
		assert(SUCCEEDED(hr));
		return std::move(resource);
	}


	void ShaderEffectProcessor::Init()
	{
		WinApp *winApp = WinApp::GetInstance();
		auto device = GetDevice();
		auto srvDescHeap = GetDescHeapCbvSrvUav();

#ifdef _DEBUG

		//assert(device and "Deviceがnullptrです");

#endif // _DEBUG

		// RTVの作成
		rtvDescHeap_ = std::make_unique<DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>>(device, kTextureCount_, false);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 出力結果をSRGBに変換して書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む

		// SRVの作成
		srvHeapRange_ = srvDescHeap->RequestHeapAllocation(kTextureCount_);
		D3D12_SHADER_RESOURCE_VIEW_DESC renderTexturSrvDesc{};
		renderTexturSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTexturSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		renderTexturSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		renderTexturSrvDesc.Texture2D.MipLevels = 1u;

		// 描画先のテクスチャ
		for (uint32_t i = 0; i < kTextureCount_; i++) {
			auto &texture = fullScreenTexture_[i];
			texture = OffScreenRenderer::CreateRenderTextrueResource(device, winApp->kWindowWidth, winApp->kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0xFF0000FF);
			device->CreateRenderTargetView(texture.Get(), &rtvDesc, rtvDescHeap_->GetHandle(0, i).cpuHandle_);
			device->CreateShaderResourceView(texture.Get(), &renderTexturSrvDesc, srvHeapRange_.GetHandle(i).cpuHandle_);
		}


	}

	void ShaderEffectProcessor::Input(ID3D12Resource *const resource)
	{
		CopyTexture(resource, fullScreenTexture_[textureTarget_].Get());
	}

	void ShaderEffectProcessor::GetResult(ID3D12Resource *const result) const
	{
		CopyTexture(fullScreenTexture_[textureTarget_].Get(), result);
	}

	void ShaderEffectProcessor::CopyTexture(ID3D12Resource *const src, ID3D12Resource *const dst)
	{
		auto command = GetCommandList();

		{
			auto srcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(src, D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_COPY_SOURCE);
			auto dstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dst, D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_COPY_DEST);

			command->ResourceBarrier(2, std::array<D3D12_RESOURCE_BARRIER, 2>{ srcBarrier, dstBarrier }.data());
		}

		command->CopyResource(dst, src);

		{
			auto srcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(src, D3D12_RESOURCE_STATE_COPY_SOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
			auto dstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dst, D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_RENDER_TARGET);

			command->ResourceBarrier(2, std::array<D3D12_RESOURCE_BARRIER, 2>{ srcBarrier, dstBarrier }.data());
		}

	}


	void FullScreenRenderer::Init(const std::list<std::pair<std::wstring, std::wstring>> &key)
	{

		//param_->fValue_ = { 16.f, 0.8f };
		auto device = GetDevice();


		//std::array<D3D12_ROOT_PARAMETER, 2u> rootParameters = {};

#pragma region Texture

		//// DescriptorRangeの設定
		//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		//descriptorRange[0].BaseShaderRegister = 0;                                                   // 0から始める
		//descriptorRange[0].NumDescriptors = 1;                                                       // 数は1つ
		//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
		//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

		//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      // DescriptorTableを使う
		//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                // PixelShaderで使う
		//rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;             // Tableの中身の配列を指定
		//rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで使用する数

		//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		//rootParameters[1].Descriptor.ShaderRegister = 0;                     // レジスタ番号1とバインド

		//SolEngine::RootParameters<>{};

#pragma endregion

#pragma region RasterizerState(ラスタライザステート)

		// RasterizerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

		auto sampler = SolEngine::ResourceSource<RootSignature>::DefaultSampler();
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

		auto *const rootSignatureManager = SolEngine::ResourceObjectManager<RootSignature>::GetInstance();
		SolEngine::ResourceSource<RootSignature> rootSignatureSource{ .sampler_ = sampler, .item_ = { "t0PS,b0PS" } };

		rootSignature_ = rootSignatureManager->Load(rootSignatureSource);

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = nullptr;
		inputLayoutDesc.NumElements = 0u;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_.GetResource()->Get();	// RootSignature
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;			// RasterizeState

		// DSVのFormatを設定する
		graphicsPipelineStateDesc.DepthStencilState = { .DepthEnable = false };

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

		auto *pShaderManager_ = SolEngine::ResourceObjectManager<Shader>::GetInstance();

		for (const auto &filePath : key) {

			auto &pileLine = pipelineState_[filePath];

			auto vsShader = pShaderManager_->Load({ filePath.first, L"vs_6_0" });
			auto psShader = pShaderManager_->Load({ filePath.second, L"ps_6_0" });

			SolEngine::PipelineState::ShaderSet copyShader;
			copyShader.vertex_ = vsShader;
			copyShader.pixel_ = psShader;

			copyShader.SetPipelineDesc(&graphicsPipelineStateDesc);

			hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pileLine));

			assert(SUCCEEDED(hr));
		}
	}

	void FullScreenRenderer::Draw(const std::pair<std::wstring, std::wstring> &key, ID3D12Resource *texture, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{

		auto command = GetCommandList();

#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrierF{};
		// 今回のバリアはTransition
		barrierF.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrierF.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。
		barrierF.Transition.pResource = texture;
		// 遷移前(現在)のResourceState
		barrierF.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// 遷移後のResourceState
		barrierF.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// TransitionBurrierを張る
		command->ResourceBarrier(1, &barrierF);

#pragma endregion


		command->SetGraphicsRootSignature(GetRootSignature());
		command->SetPipelineState(GetPipeLine(key));
		command->SetGraphicsRootDescriptorTable(0, gpuHandle);
		//command->SetGraphicsRootConstantBufferView(accesser_.GetIndex<CBuffer<ValuePair>>(), param_.GetGPUVirtualAddress());

		command->DrawInstanced(3, 1, 0, 0);


#pragma region TransitionBarrierを張る

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrierS{};
		// 今回のバリアはTransition
		barrierS.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrierS.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。
		barrierS.Transition.pResource = texture;
		// 遷移前(現在)のResourceState
		barrierS.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// 遷移後のResourceState
		barrierS.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBurrierを張る
		command->ResourceBarrier(1, &barrierS);

#pragma endregion

	}
}