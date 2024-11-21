#pragma once

namespace SoLib {

	class TextureConverter {
	public:
		TextureConverter() = default;

		/// @brief テクスチャをWICからDDSに変換する
		/// @param file_path ファイルパス
		void ConvertTextureWIC2DDS(const char *const file_path);


	private:

		/// @brief テクスチャのロード
		/// @param file_path 
		void WICTextureLoad(const char *const file_path);

		void Convert();

	};

}