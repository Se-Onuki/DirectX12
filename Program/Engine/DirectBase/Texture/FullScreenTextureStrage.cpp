/// @file FullScreenTextureStrage.cpp
/// @brief 全画面テクスチャのストレージ
/// @author ONUKI seiya
#include "FullScreenTextureStrage.h"
#include "../PostEffect/OffScreenRendering.h"

namespace SolEngine {
	std::unique_ptr<FullScreenTexture> SolEngine::FullScreenTexture::Create(const DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>::Handle &rtvHandle, const DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::Handle &srvHandle)
	{
		WinApp *winApp = WinApp::GetInstance();
		auto device = GetDevice();
		std::unique_ptr<FullScreenTexture> result = std::make_unique<FullScreenTexture>();

		result->rtvHandle_ = rtvHandle;
		result->srvHandle_ = srvHandle;

		//
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,      // 出力結果をSRGBに変換して書き込む
			.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D	// 2Dテクスチャとして書き込む
		};

		// 描画先のテクスチャ
		result->renderTargetTexture_ = PostEffect::OffScreenRenderer::CreateRenderTextrueResource(device, winApp->kWindowWidth, winApp->kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, result->clearColor_);

		device->CreateRenderTargetView(result->renderTargetTexture_.Get(), &rtvDesc, result->rtvHandle_.cpuHandle_);

		// SRVの作成
		D3D12_SHADER_RESOURCE_VIEW_DESC renderTexturSrvDesc_{};
		renderTexturSrvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTexturSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		renderTexturSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		renderTexturSrvDesc_.Texture2D.MipLevels = 1u;

		device->CreateShaderResourceView(result->renderTargetTexture_.Get(), &renderTexturSrvDesc_, result->srvHandle_.cpuHandle_);
		return std::move(result);
	}
}