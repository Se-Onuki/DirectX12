#pragma once
#include <cstdint>
#include <string>
#include "../../Model/Mesh.h"

namespace SolEngine {

	class SkyBox
	{
	public:
		SkyBox() { CreateBuffer(); };

		~SkyBox() = default;

		/// @brief テクスチャの割当て
		/// @param handle テクスチャハンドル
		void SetTexture(uint32_t handle) { textureHandle_ = handle; }
		/// @brief テクスチャの割当て
		/// @param file_path ファイルパス
		void SetTexture(const std::string &file_path);

	private:
		/// @brief バッファの構築
		void CreateBuffer();
		// 頂点データ
		VertexBuffer<ModelVertexData::VertexData> vertex_;
		// Index情報
		IndexBuffer<uint32_t> index_;
		// テクスチャハンドル
		uint32_t textureHandle_;

	};
}