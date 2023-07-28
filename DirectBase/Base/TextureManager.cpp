#include "TextureManager.h"

#include "../../externals/DirectXTex/DirectXTex.h"
#include "../../Header/Texture/Texture.h"
#include "../../Header/Descriptor/DescriptorHandIe.h"
#include "../../Header/Render/Render.hpp"

void TextureManager::StartDraw()
{
}

uint32_t TextureManager::Load(const std::string &file_name)
{
	return GetInstance()->LoadInternal(file_name);
}

void TextureManager::Init(ID3D12Device *const device, ID3D12GraphicsCommandList *const commandList, const std::string &directoryPath) {

	device_ = device;
	directoryPath_ = directoryPath;
	commandList_ = commandList;

	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Reset();
}

void TextureManager::Reset() {
	//HRESULT hr = S_FALSE;

	nextIndex_ = alreadyUsedCount;
	srvHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxTextureCount, true);

	for (size_t i = alreadyUsedCount; i < maxTextureCount; i++) {
		textureArray_[i].textureResource.Reset();
		textureArray_[i].cpuHandleSRV.ptr = 0;
		textureArray_[i].gpuHandleSRV.ptr = 0;
		textureArray_[i].name.clear();
	}
}

void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParamIndex, uint32_t textureHandle) const {
	commandList_->SetGraphicsRootDescriptorTable(rootParamIndex, textureArray_[textureHandle].gpuHandleSRV);		// TextureのSRVテーブル情報を設定
}

void TextureManager::EndFlame()
{
	intermediateData_.clear();
}

uint32_t TextureManager::LoadInternal(const std::string &file_name)
{
#pragma region Texture検索

	auto it = std::find_if(textureArray_.begin(), textureArray_.end(), [&](const auto &texture) {
		return texture.name == file_name;
		}
	);
	if (it != textureArray_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		return static_cast<uint32_t>(std::distance(textureArray_.begin(), it));
		//return handle;
	}

#pragma endregion

	assert(nextIndex_ < maxTextureCount);
	assert(nextIndex_ >= alreadyUsedCount);
	uint32_t handle = nextIndex_;
	Texture &texture = textureArray_[handle];

#pragma region Textureを読んで転送する

	// Textureを読んで転送する
	DirectX::ScratchImage mipImage;

	mipImage = TextureFunc::Load(directoryPath_ + file_name);


	const DirectX::TexMetadata &metadata = mipImage.GetMetadata();
	texture.textureResource = TextureFunc::CreateResource(device_, metadata);
	intermediateData_.push_back(TextureFunc::UpdateData(texture.textureResource.Get(), mipImage, device_, commandList_));


#pragma endregion

#pragma region ShaderResourceViewを作る


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	texture.name = file_name;
	texture.cpuHandleSRV = DescriptorHandIe::GetCPUHandle(srvHeap_.Get(), descriptorSizeSRV_, handle);
	texture.gpuHandleSRV = DescriptorHandIe::GetGPUHandle(srvHeap_.Get(), descriptorSizeSRV_, handle);

	// SRVの作成
	device_->CreateShaderResourceView(texture.textureResource.Get(), &srvDesc, texture.cpuHandleSRV);

#pragma endregion

	//file_name;
	nextIndex_++;
	return handle;
}
