#pragma once
#include "../../externals/DirectXTex/DirectXTex.h"
#include <string>
#include <d3d12.h>
#include "../String/String.h"
#include "../../externals/DirectXTex/d3dx12.h"
#include "../Create/Create.h"
#include <wrl.h>

namespace TextureFunc
{
	/// @brief TextureをCPUで読み込む
	/// @param file_path ファイルパス
	/// @return ミップマップ付きのデータ
	inline DirectX::ScratchImage Load(const std::string &file_path) {
		// テクスチャファイルを呼んでプログラムで扱えるようにする
		DirectX::ScratchImage image{};
		std::wstring file_pathW = ConvertString(file_path);
		HRESULT hr = DirectX::LoadFromWICFile(file_pathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));

		// ミップマップの作成
		DirectX::ScratchImage mipImages{};
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));

		// ミップマップ付きのデータを返す
		return mipImages;
	}

	/// @brief DirectX12のTextureResourceを作る
	/// @param device デバイス
	/// @param metadata テクスチャメタデータ
	/// @return テクスチャリソースデータ
	inline Microsoft::WRL::ComPtr<ID3D12Resource> CreateResource(ID3D12Device *device, const DirectX::TexMetadata &metadata) {
		// 1. metadataを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width);				// textureの幅
		resourceDesc.Height = UINT(metadata.height);			// textureの高さ
		resourceDesc.MipLevels = UINT16(metadata.mipLevels);	// mipMapの数
		resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);	// 奥行き or 配列Textureの配列数
		resourceDesc.Format = metadata.format;					// TextureのFormat
		resourceDesc.SampleDesc.Count = 1;						// サンプリングカウント
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Textureの次元数。普段使っているのは2次元。

		// 2. 利用するHeapの設定。
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作成


		// 3. Resourceを生成する
		Microsoft::WRL::ComPtr<ID3D12Resource>resource = nullptr;
		HRESULT hr = S_FALSE;
		hr = device->CreateCommittedResource(
			&heapProperties,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&resourceDesc,						// Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
			nullptr,							// Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource));			// 作成するResourceへのポインタ
		assert(SUCCEEDED(hr));
		return resource;
	}

	inline [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> UpdateData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages, ID3D12Device *device, ID3D12GraphicsCommandList *commandList) {
		//// Meta情報を取得
		//const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
		//// 全MipMapについて
		//for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//	// MipMapLevelを指定して各Imageを取得
		//	const DirectX::Image *img = mipImages.GetImage(mipLevel, 0, 0);
		//	// Textureに転送
		//	HRESULT  hr = texture->WriteToSubresource(
		//		UINT(mipLevel),
		//		nullptr,				// 全領域へコピー
		//		img->pixels,			// 元データアドレス
		//		UINT(img->rowPitch),	// 1ラインサイズ
		//		UINT(img->slicePitch)	// 1枚サイズ
		//	);
		//	assert(SUCCEEDED(hr));
		//}

#pragma region IntermediateResource(中間リソース)

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
		Microsoft::WRL::ComPtr<ID3D12Resource>intermediateResource = CreateBufferResource(device, intermediateSize);

#pragma endregion

#pragma region データ転送をコマンドに積む

		UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

#pragma endregion

#pragma region ResourceStateを変更し, InterMediateResourceを返す

		// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		commandList->ResourceBarrier(1, &barrier);
		return intermediateResource;

#pragma endregion

	}
	inline [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource>CreateDepthStencilTextureResource(ID3D12Device *device, int32_t width, int32_t height) {

#pragma region Resource/Heapの設定を行う

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width;										// Textureの幅
		resourceDesc.Height = height;									// Textureの高さ
		resourceDesc.MipLevels = 1;										// mipmapの数
		resourceDesc.DepthOrArraySize = 1;								// 奥行き or 配列Textureの配列数
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// DepthStencilとして利用可能なフォーマット
		resourceDesc.SampleDesc.Count = 1;								// サンプリングカウント。1固定。
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 二次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// DepthStencilとして使う通知

		// 利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;			// VRAM上に作る

#pragma endregion

#pragma region 深度のクリア最適化設定

		// 深度のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.f;				// 1.f(最大値)でクリア
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resourceと合わせる。

#pragma endregion

#pragma region Resourceの生成

		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		HRESULT hr = S_FALSE;
		hr = S_FALSE;
		hr = device->CreateCommittedResource(
			&heapProperties,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&resourceDesc,						// Resourceの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値を書き込む状態にしておく
			&depthClearValue,					// Clear最大値
			IID_PPV_ARGS(&resource)				// 作成するResourceポインタへのポインタ
		);
		assert(SUCCEEDED(hr));

		return resource;
#pragma endregion

	}

};
