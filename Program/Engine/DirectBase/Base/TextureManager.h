/// @file TextureManager.h
/// @brief テクスチャの管理を行う
/// @author ONUKI seiya
#pragma once
#include <cstdint>
#include <d3d12.h>
#include <numbers>
#include <wrl.h>

#include <array>
#include <list>
#include <string>

#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "../Descriptor/DescriptorManager.h"
#include "../Engine/Utils/Math/Math.hpp"

class TextureManager
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	TextureManager() = default;
	TextureManager(const TextureManager &) = delete;
	TextureManager operator=(const TextureManager &) = delete;
	~TextureManager() = default;

public:
	static const uint32_t maxTextureCount = 128u;

	struct Texture
	{
		ComPtr<ID3D12Resource> textureResource_;
		DescHeapCbvSrvUav::Handle handle_;
		Vector2 textureSize_;
		std::string name_;
	};

	/// @brief テクスチャのハンドル
	/// @details 実質的にはuint32_t型のラッパー
	struct TextureHandle
	{

		TextureHandle() = default;
		TextureHandle(const TextureHandle &) = default;
		TextureHandle(uint32_t index) : index_(index) {}

		friend TextureManager;

		uint32_t index_;
		/// テクスチャのデータを返す
		const Texture *operator*() const { return GetTexture(); }
		/// テクスチャのデータを返す
		const Texture *operator->() const { return GetTexture(); }

		/// @brief テクスチャのデータを取得する
		/// @return テクスチャへのアドレス
		/// @details Indexを使って､TextureManagerからテクスチャを取得する
		const Texture *GetTexture() const;

		/// @brief そのテクスチャが存在しているか
		/// @return 存在していないなら偽
		explicit operator bool() const { return GetTexture(); }

	private:
		// テクスチャマネージャへのアクセッサ
		static const TextureManager *pTextureManager_;
	};

	static inline TextureManager *const GetInstance()
	{
		static TextureManager instance{};
		TextureHandle::pTextureManager_ = &instance;
		return &instance;
	}

	/// @brief 初期テクスチャの読み込み
	/// @return 初期テクスチャのインデックス
	static uint32_t LoadDefaultTexture() { return Load("white2x2.png"); }

	/// @brief 描画開始
	void StartDraw();
	/// @brief テクスチャの読み込み
	/// @param[in] file_name 読み込むファイル名
	/// @return 読み込んだテクスチャのインデックス
	static uint32_t Load(const std::string_view &file_name);
	/// @param[in] textData バイナリデータ
	static uint32_t Load(const std::string_view &file_name, const std::span<std::byte> &textData);
	/// @brief 初期化
	/// @param[in] device デバイス
	/// @param[in] commandList コマンドリスト
	/// @param[in] directoryPath ディレクトリパスの設定
	void Init(ID3D12Device *const device, ID3D12GraphicsCommandList *const commandList, const std::string &directoryPath = "resources/");

	/// @brief リセット
	/// @details すべてのテクスチャを開放する
	void Reset();

	/// @brief ImGuiウィンドウ
	void ImGuiWindow();

	/// @brief ImGuiテクスチャセレクター
	/// @details ImGuiによって選択されたテクスチャを返す
	/// @return 選択されたテクスチャのインデックス
	uint32_t ImGuiTextureSelecter(uint32_t index);

	/// @brief リソースDescの取得
	/// @param[in] index インデックス
	/// @return リソースDesc
	inline D3D12_RESOURCE_DESC GetResourceDesc(uint32_t index)
	{
		return textureArray_.at(index).textureResource_->GetDesc();
	}
	/// @brief SRVハンドルの取得
	/// @param[in] index インデックス
	/// @return SRVハンドル
	inline const D3D12_GPU_DESCRIPTOR_HANDLE &GetGpuSrvHandle(uint32_t index)
	{
		return textureArray_.at(index).handle_.gpuHandle_;
	}

	/// @brief テクスチャの取得
	/// @return テクスチャへのアドレス(空の場合 nullptr)
	const Texture *GetTexture(uint32_t index) const
	{
		// 対象のテクスチャ
		const auto &texture = textureArray_.at(index);

		// テクスチャが存在していないなら､nullptrを返す
		if (texture.name_.empty()) {
			return nullptr;
		}

		// それ以外の場合は､テクスチャのアドレスを返す
		return &texture;
	}

	/// @brief SRVテーブル情報を設定する
	/// @param[in] rootParamIndex ルートパラメーターインデックス
	/// @param[in] textureHandle テクスチャのインデックス
	void SetGraphicsRootDescriptorTable(UINT rootParamIndex, uint32_t textureHandle) const;

	/// @brief 描画終了
	void EndFlame();

private:
	/// @brief ファイル名からテクスチャを生成する｡
	/// @param file_name ファイル名
	/// @return 生成されたテクスチャのHandle
	uint32_t LoadInternal(const std::string_view &file_name);

	/// @brief バイナリデータからテクスチャを生成する｡
	/// @param file_name ファイル名
	/// @param textData バイナリデータ( ARGB形式 )
	/// @return 生成されたテクスチャのHandle
	uint32_t LoadInternal(const std::string_view &file_name, const std::span<std::byte> &textData);

	// デバイス(借用)
	ID3D12Device *device_ = nullptr;
	// コマンドリスト(借用)
	ID3D12GraphicsCommandList *commandList_ = nullptr;
	// デスクリプタヒープ(借用)
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV> *srvHeap_ = nullptr;
	// ヒープの使用位置
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_{};

	// デスクリプタヒープを現在使用している量(size)
	uint32_t nextIndex_ = (std::numeric_limits<uint32_t>::max)();

	// 根底ディレクトリ
	std::string directoryPath_;

	// テクスチャ配列
	std::array<Texture, maxTextureCount> textureArray_ = {};

	// 画像転送用一時テクスチャリソース
	std::list<ComPtr<ID3D12Resource>> intermediateData_;
};

namespace SoLib {
	template <>
	bool ImGuiWidget(const char *const label, TextureManager::Texture *const value);
}

namespace SolEngine {

	class Texture : public IResourceObject
	{
		template <class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		struct TextureData
		{
			ComPtr<ID3D12Resource> textureResource;
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleSRV;
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleSRV;
			std::string name;
		};
	};

	template <>
	class ResourceSource<Texture>
	{
	public:
		std::string filePath_;

		/// @brief 文字列化する
		/// @return 文字列
		const std::string &ToStr() const { return filePath_; }

		bool operator==(const ResourceSource<Texture> &) const = default;
	};

	template <>
	class ResourceCreater<Texture>
	{
		template <class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		/// @brief オブジェクトの生成
		/// @param[in] source ソースデータ
		/// @return 生成されたオブジェクト
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
		std::list<ComPtr<ID3D12Resource>> intermediateData_;
	};

}

/// @brief テクスチャハンドルの短縮リテラル
using TextureHandle = TextureManager::TextureHandle;

namespace std {

	template <>
	struct hash<SolEngine::ResourceSource<SolEngine::Texture>>
	{
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Texture> &data) const
		{
			return std::hash<std::string>{}(data.filePath_);
		}
	};

	template <>
	struct hash<TextureHandle>
	{
		size_t operator()(const TextureHandle &data) const
		{
			return std::hash<uint32_t>{}(data.index_);
		}
	};
}

namespace SolEngine {
	using TextureManager = ResourceObjectManager<Texture, ResourceSource<Texture>, ResourceCreater<Texture>, 256>;
}