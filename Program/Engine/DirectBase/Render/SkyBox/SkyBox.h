#pragma once
#include <cstdint>
#include <string>
#include "../../Model/Mesh.h"

namespace SolEngine {

	class SkyBox
	{
	public:
		SkyBox() = default;

		~SkyBox() = default;

		/// @brief テクスチャの割当て
		/// @param handle テクスチャハンドル
		void SetTexture(uint32_t handle) { textureHandle_ = handle; }
		/// @brief テクスチャの割当て
		/// @param file_path ファイルパス
		void SetTexture(const std::string &file_path);

		uint32_t GetTextureHandle() const { return textureHandle_; }

	private:
		// テクスチャハンドル
		uint32_t textureHandle_;

	};

	class SkyBoxRender : public SoLib::Singleton<SkyBoxRender>, public SolEngine::EngineObject {

		SkyBoxRender() = default;
		SkyBoxRender(const SkyBoxRender &) = delete;
		SkyBoxRender &operator=(const SkyBoxRender &) = delete;
		~SkyBoxRender() = default;
		friend SoLib::Singleton<SkyBoxRender>;
	public:
		/// @brief 初期化処理
		void Init();

		void Draw(const SkyBox &skyBox, const Transform &transform, const Camera3D &camera) const;

	private:
		/// @brief バッファの構築
		void CreateBuffer();

	private:
		// 頂点データ
		VertexBuffer<Vector4> vertex_;
		// Index情報
		IndexBuffer<uint32_t> index_;

		// ルートシグネチャ
		ResourceHandle<RootSignature> rootSignature_;

		ComPtr<ID3D12PipelineState> pipeline_;

	};
}