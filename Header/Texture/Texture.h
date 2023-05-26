#pragma once
#include "../../externals/DirectXTex/DirectXTex.h"
#include <string>
#include <d3d12.h>
#include "../String/String.hpp"

namespace Texture
{
	/// @brief TextureをCPUで読み込む
	/// @param file_path ファイルパス
	/// @return ミップマップ付きのデータ
	DirectX::ScratchImage Load(const std::string &file_path) {
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
	ID3D12Resource *CreateResource(ID3D12Device *device, const DirectX::TexMetadata &metadata) {
		// 1. metadataを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width);				// textureの幅
		resourceDesc.Height = UINT(metadata.height);			// textureの高さ
		resourceDesc.MipLevels = UINT16(metadata.mipLevels);	// mipMapの数
		resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);	// 奥行き or 配列Textureの配列数
		resourceDesc.Format = metadata.format;					// TextureのFormat
		resourceDesc.SampleDesc.Count = 1;						// サンプリングカウント
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Textureの次元数。普段使っているのは2次元。

		// 2. 利用するHeapの設定。非常に特殊な運用。02_04_exで一般的なケースがある。
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;							// 細かい設定を行う
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// WriteBackポリシーでCPUアクセス可能
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;				// プロセッサの近くに配置

		// 3. Resourceを生成する
		ID3D12Resource *resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&resourceDesc,						// Resourceの設定
			D3D12_RESOURCE_STATE_GENERIC_READ,	// 初回のResourceState。Textureは基本読むだけ。
			nullptr,							// Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource));			// 作成するResourceへのポインタ
		assert(SUCCEEDED(hr));
		return resource;
	}

	/// @brief TextureResourceにデータを転送する
	/// @param texture テクスチャリソースデータ
	/// @param mipImages ミップマップデータ
	void UpdateData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages) {
		// Meta情報を取得
		const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
		// 全MipMapについて
		for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
			// MipMapLevelを指定して各Imageを取得
			const DirectX::Image *img = mipImages.GetImage(mipLevel, 0, 0);
			// Textureに転送
			HRESULT  hr = texture->WriteToSubresource(
				UINT(mipLevel),
				nullptr,				// 全領域へコピー
				img->pixels,			// 元データアドレス
				UINT(img->rowPitch),	// 1ラインサイズ
				UINT(img->slicePitch)	// 1枚サイズ
			);
			assert(SUCCEEDED(hr));
		}
	}

};
