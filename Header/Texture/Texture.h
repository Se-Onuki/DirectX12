#pragma once
#include "../../externals/DirectXTex/DirectXTex.h"
#include <string>
#include <d3d12.h>
#include "../String/String.hpp"
#include "../../externals/DirectXTex/d3dx12.h"
#include "../Create/Create.h"

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
			D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
			nullptr,							// Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource));			// 作成するResourceへのポインタ
		assert(SUCCEEDED(hr));
		return resource;
	}


	[[nodiscard]] ID3D12Resource *UpdateData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages,ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
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
		ID3D12Resource *intermediateResource = CreateBufferResource(device, intermediateSize);

#pragma endregion

#pragma region データ転送をコマンドに積む

		UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

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

};
