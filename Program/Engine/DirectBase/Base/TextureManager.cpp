/// @file TextureManager.cpp
/// @brief テクスチャの管理を行う
/// @author ONUKI seiya
#include "TextureManager.h"

#include "../../externals/DirectXTex/DirectXTex.h"
#include "../../DirectBase/Texture/Texture.h"
#include "../../DirectBase/Descriptor/DescriptorHandle.h"
#include "../../DirectBase/Render/Render.h"

#include "DirectXCommon.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include "../../Engine/Utils/SoLib/SoLib_ImGui.h"

const TextureManager *TextureManager::TextureHandle::pTextureManager_ = nullptr;

void TextureManager::StartDraw()
{

#pragma region 描画用DescriptorHeapの設定

	// 描画用のDescriptorHeapの設定。
	//ID3D12DescriptorHeap *descriptorHeaps[] = { srvHeap_.Get() };
	//commandList_->SetDescriptorHeaps(1, descriptorHeaps);
	srvHeap_->SetCommand(commandList_, 1);

#pragma endregion

}

uint32_t TextureManager::Load(const std::string_view &file_name)
{
	return GetInstance()->LoadInternal(file_name);
}


uint32_t TextureManager::Load(const std::string_view &file_name, const std::span<std::byte> &textData)
{
	return GetInstance()->LoadInternal(file_name, textData);
}

void TextureManager::Init(ID3D12Device *const device, ID3D12GraphicsCommandList *const commandList, const std::string &directoryPath) {

	device_ = device;
	directoryPath_ = directoryPath;
	commandList_ = commandList;
	srvHeap_ = DirectXCommon::GetInstance()->GetSRVHeap();

	heapRange_ = srvHeap_->RequestHeapAllocation(maxTextureCount);

	Reset();
}

void TextureManager::Reset() {

	nextIndex_ = 0;

	for (size_t i = 0u; i < maxTextureCount; i++) {
		textureArray_[i].textureResource_.Reset();
		textureArray_[i].handle_.cpuHandle_.ptr = 0;
		textureArray_[i].handle_.gpuHandle_.ptr = 0;
		textureArray_[i].name_.clear();
	}
}

void TextureManager::ImGuiWindow() {
	ImGui::Begin("TextureManager");

	static uint32_t index = LoadInternal("white2x2.png");
	index = ImGuiTextureSelecter(index);
	ImGui::End();
}

uint32_t TextureManager::ImGuiTextureSelecter(uint32_t index) {
	static std::string filePath;
	ImGui::InputTextWithHint("textureLoad", "resources/...", &filePath);
	if (ImGui::Button("Load")) {
		LoadInternal(filePath);
		filePath.clear();
	}

	index = SoLib::ImGuiWidget("TextureList", &textureArray_, index,
		[this](const decltype(index) &itemIndex)->std::string
		{
			if (textureArray_[itemIndex].name_.empty()) { return ""; }
			return textureArray_[itemIndex].name_ + "[" + std::to_string(itemIndex) + "]";
		}
	);

	SoLib::ImGuiWidget(textureArray_[index].name_.c_str(), &textureArray_[index]);

	return index;
}

void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParamIndex, uint32_t textureHandle) const {
	commandList_->SetGraphicsRootDescriptorTable(rootParamIndex, textureArray_[textureHandle].handle_.gpuHandle_);		// TextureのSRVテーブル情報を設定
}

void TextureManager::EndFlame()
{
	intermediateData_.clear();
}

uint32_t TextureManager::LoadInternal(const std::string_view &file_name)
{
#pragma region Texture検索

	auto it = std::find_if(textureArray_.begin(), textureArray_.end(), [&](const auto &texture)
		{
			return texture.name_ == file_name;
		}
	);
	if (it != textureArray_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		return static_cast<uint32_t>(std::distance(textureArray_.begin(), it));
	}

#pragma endregion

	assert(nextIndex_ < maxTextureCount);
	uint32_t handle = nextIndex_;
	Texture &texture = textureArray_[handle];

#pragma region Textureを読んで転送する

	// Textureを読んで転送する
	DirectX::ScratchImage mipImage;

	std::string file = directoryPath_;
	file.append(file_name);

	mipImage = SolEngine::TextureFunc::Load(file);


	const DirectX::TexMetadata &metadata = mipImage.GetMetadata();
	texture.textureResource_ = SolEngine::TextureFunc::CreateResource(device_, metadata);
	intermediateData_.push_back(SolEngine::TextureFunc::UpdateData(texture.textureResource_.Get(), mipImage, device_, commandList_));


#pragma endregion

#pragma region ShaderResourceViewを作る


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}

	auto textureDesc = texture.textureResource_->GetDesc();

	texture.name_ = file_name;
	texture.handle_ = heapRange_.GetHandle(handle);
	texture.textureSize_ = Vector2{ static_cast<float>(textureDesc.Width), static_cast<float>(textureDesc.Height) };

	// SRVの作成
	device_->CreateShaderResourceView(texture.textureResource_.Get(), &srvDesc, texture.handle_.cpuHandle_);

#pragma endregion

	//file_name;
	nextIndex_++;
	return handle;
}

uint32_t TextureManager::LoadInternal(const std::string_view &file_name, const std::span<std::byte> &texData)
{
#pragma region Texture検索

	auto it = std::find_if(textureArray_.begin(), textureArray_.end(), [&](const auto &texture)
		{
			return texture.name_ == file_name;
		}
	);
	if (it != textureArray_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		return static_cast<uint32_t>(std::distance(textureArray_.begin(), it));
		//return handle;
	}

#pragma endregion

	assert(nextIndex_ < maxTextureCount);
	uint32_t handle = nextIndex_;
	Texture &texture = textureArray_[handle];

#pragma region Textureを読んで転送する

	// Textureを読んで転送する
	DirectX::ScratchImage mipImage;

	mipImage = SolEngine::TextureFunc::Load(texData);


	const DirectX::TexMetadata &metadata = mipImage.GetMetadata();
	texture.textureResource_ = SolEngine::TextureFunc::CreateResource(device_, metadata);
	intermediateData_.push_back(SolEngine::TextureFunc::UpdateData(texture.textureResource_.Get(), mipImage, device_, commandList_));


#pragma endregion

#pragma region ShaderResourceViewを作る


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	auto textureDesc = texture.textureResource_->GetDesc();

	texture.name_ = file_name;
	texture.handle_ = heapRange_.GetHandle(handle);
	texture.textureSize_ = Vector2{ static_cast<float>(textureDesc.Width), static_cast<float>(textureDesc.Height) };

	// SRVの作成
	device_->CreateShaderResourceView(texture.textureResource_.Get(), &srvDesc, texture.handle_.cpuHandle_);

#pragma endregion

	//file_name;
	nextIndex_++;
	return handle;
}

const TextureManager::Texture *TextureManager::TextureHandle::GetTexture() const
{
	return pTextureManager_->GetTexture(index_);
}

template<>
bool SoLib::ImGuiWidget(const char *const label, TextureManager::Texture *const value)
{
#ifdef USE_IMGUI

	// もし値がnullptrなら､falseを返す
	if (value == nullptr or label == nullptr) {
		return false;
	}

	ImGui::Text(label);

	if (value->handle_.gpuHandle_.ptr) {
		const auto &resourceDesc = value->textureResource_->GetDesc();
		const Vector2 texSize = Vector2{ (float)resourceDesc.Width,(float)resourceDesc.Height } / (float)resourceDesc.Width * 100.f;
		ImGui::Image(value->GetTextureID(), ImVec2{ texSize.x,texSize.y });
		ImGui::SameLine();
		ImGui::Text("Width: %d\nHeight: %d", resourceDesc.Width, resourceDesc.Height);
	}

	return false;
#else 

	label; value;

	return false;

#endif // USE_IMGUI
}