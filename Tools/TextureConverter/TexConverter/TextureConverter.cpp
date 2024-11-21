#include "TextureConverter.h"
#include "../StringConverter/StringConverter.h"

void SoLib::TextureConverter::ConvertTextureWIC2DDS(const char *const file_path)
{
	// テクスチャファイルを読み込む
	WICTextureLoad(file_path);

	// DDS形式に変換して書き出す

}

void SoLib::TextureConverter::WICTextureLoad(const char *const file_path)
{
	// ファイルパスをワイド文字列に変換
	const std::wstring wFile_Path = ConvertString(file_path);

	// テクスチャ読み込み


}
