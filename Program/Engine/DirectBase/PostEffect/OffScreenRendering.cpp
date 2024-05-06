#include "OffScreenRendering.h"

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
}