#pragma once
#include "DirectXTex/DirectXTex.h"
#include <string>
#include <span>

namespace SoLib {

	class TextureConverter {
	public:

		// コマンドを解釈する
		struct CommandLoader {
			// ミップレベル
			size_t mipLevel_ = 0u;

			CommandLoader() = default;
			CommandLoader(const std::span<char *> options);
		};

		struct TextureFileName {
			// ディレクトリパス
			std::wstring directoryPath_;
			// ファイル名
			std::wstring fileName_;
			// ファイル拡張子
			std::wstring fileExt_;

			/// @brief ファイル名を構築する
			/// @return 結合したファイル名
			std::wstring GetFilePath() const {
				return directoryPath_ + fileName_ + fileExt_;
			}
			/// @brief ファイル名を構築する
			/// @param fileExt 拡張子
			/// @return 結合したファイル名
			std::wstring GetFilePath(const std::wstring_view &fileExt) const {
				return directoryPath_ + fileName_ + fileExt.data();
			}
		};
		TextureConverter() = default;

		/// @brief 使用方法の出力
		static void OutputUsage();

		/// @brief テクスチャをWICからDDSに変換する
		/// @param file_path ファイルパス
		void ConvertTextureWIC2DDS(const std::string_view &file_path, std::span<char *> options);


	private:

		/// @brief テクスチャのロード
		/// @param file_path ファイルパス
		void LoadWICTexture(const std::string_view &file_path);

		/// @brief DDSファイルへ変換して保存する
		void SaveDDSTexture();

		/// @brief パスの分解
		/// @param file_path ファイルパス
		/// @return ファイルパス
		TextureFileName SeparateFilePath(const std::wstring_view &file_path) const;

		// テクスチャファイルの名前
		TextureFileName textureFileName_;

		// コマンドのデータ
		CommandLoader command_;

		// 画像データ
		DirectX::TexMetadata metaData_;
		// 画像イメージのコンテナ
		DirectX::ScratchImage scratchImage_;

	};

}