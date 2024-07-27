#pragma once
#include <wrl.h>
#include <cstdint>
#include <d3d12.h>

#include <array>
#include <string>
#include <list>

#include "../Descriptor/DescriptorManager.h"
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

class TextureManager
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	TextureManager() = default;
	TextureManager(const TextureManager &) = delete;
	TextureManager operator=(const TextureManager &) = delete;
	~TextureManager() = default;

public:

	static const uint32_t maxTextureCount = 128u;

	struct Texture {
		ComPtr<ID3D12Resource> textureResource_;
		DescHeapCbvSrvUav::Handle handle_;
		std::string name_;
	};

	static inline TextureManager *const GetInstance() {
		static TextureManager instance{};
		return &instance;
	}
	static uint32_t LoadDefaultTexture() { return Load("white2x2.png"); }

	void StartDraw();

	static uint32_t Load(const std::string &file_name);
	static uint32_t Load(const std::string &file_name, const std::span<uint8_t> &textData);

	void Init(ID3D12Device *const device, ID3D12GraphicsCommandList *const commandList, const std::string &directoryPath = "resources/");

	void Reset();

	void ImGuiWindow();

	uint32_t ImGuiTextureSelecter(uint32_t index);

	inline D3D12_RESOURCE_DESC GetResourceDesc(uint32_t index) {
		return textureArray_.at(index).textureResource_->GetDesc();
	}
	inline const D3D12_GPU_DESCRIPTOR_HANDLE &GetGpuSrvHandle(uint32_t index) {
		return textureArray_.at(index).handle_.gpuHandle_;
	}

	void SetGraphicsRootDescriptorTable(UINT rootParamIndex, uint32_t textureHandle) const;

	void EndFlame();
private:
	uint32_t LoadInternal(const std::string &file_name);

	uint32_t LoadInternal(const std::string &file_name, const std::span<uint8_t> &textData);
	// デバイス(借用)
	ID3D12Device *device_ = nullptr;
	// コマンドリスト(借用)
	ID3D12GraphicsCommandList *commandList_ = nullptr;
	// デスクリプタヒープ(借用)
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *srvHeap_;
	// ヒープの使用位置
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// デスクリプタヒープを現在使用している量(size)
	uint32_t nextIndex_ = static_cast<uint32_t>(-1);

	// 根底ディレクトリ
	std::string directoryPath_;

	// テクスチャ配列
	std::array<Texture, maxTextureCount> textureArray_ = {};

	// 画像転送用一時テクスチャリソース
	std::list<ComPtr<ID3D12Resource>>intermediateData_;
};


namespace SolEngine {

	class Texture : public IResourceObject {
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:

		struct TextureData {
			ComPtr<ID3D12Resource> textureResource;
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleSRV;
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleSRV;
			std::string name;
		};



	};

	template <>
	class ResourceSource<Texture> {
	public:
		std::string filePath_;

		const std::string &ToStr() const { return filePath_; }

		bool operator==(const ResourceSource<Texture> &) const = default;
	};

	template <>
	class ResourceCreater<Texture> {
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:

		std::unique_ptr<Texture> CreateObject(const ResourceSource<Texture> &source) const;

	private:
		// デバイス(借用)
		ID3D12Device *device_ = nullptr;
		// コマンドリスト(借用)
		ID3D12GraphicsCommandList *commandList_ = nullptr;
		// デスクリプタヒープ(借用)
		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *pSrvHeap_;
		// ヒープの使用位置
		DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;


		// 画像転送用一時テクスチャリソース
		std::list<ComPtr<ID3D12Resource>>intermediateData_;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Texture>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Texture> &data) const {
			return std::hash<std::string>{}(data.filePath_);
		}
	};
}

namespace SolEngine {
	using TextureManager = ResourceObjectManager<Texture, ResourceSource<Texture>, ResourceCreater<Texture>, 256>;
}