#include "TextureConverter.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include "../StringConverter/StringConverter.h"

namespace SoLib {
	void TextureConverter::OutputUsage()
	{
		std::cout << "画像ファイルをWIC形式からDDS形式に変換します｡\n" << std::endl;
		std::cout << "TextureConverter [ドライブ:][パス][ファイル名][-m level]\n" << std::endl;
		std::cout << "[ドライブ:][パス][ファイル名]: 変換したいWIC形式の画像ファイルを指定します｡" << std::endl;
		std::cout << "[-ml level]: ミップレベルを指定します｡初期値は0で､1pxまでの全てのミップマップを生成します｡" << std::endl;
	}

	void TextureConverter::ConvertTextureWIC2DDS(const std::string_view &file_path, std::span<char *> options)
	{
		// テクスチャファイルを読み込む
		LoadWICTexture(file_path);

		// コマンドを解釈する
		command_ = CommandLoader(options);

		// DDS形式に変換して書き出す
		SaveDDSTexture();

	}

	void TextureConverter::LoadWICTexture(const std::string_view &file_path)
	{
		HRESULT hr = S_FALSE;

		// ファイルパスをワイド文字列に変換
		const std::wstring wFile_Path = ConvertString(file_path);

		// テクスチャファイルの名前を分解
		textureFileName_ = SeparateFilePath(wFile_Path);

		// テクスチャ読み込み
		hr = DirectX::LoadFromWICFile(wFile_Path.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metaData_, scratchImage_);
		assert(SUCCEEDED(hr));


	}

	void TextureConverter::SaveDDSTexture()
	{
		HRESULT hr = S_FALSE;

		// ミップマップの保存先
		DirectX::ScratchImage mipChain;

		// ミップマップの生成
		hr = DirectX::GenerateMipMaps(scratchImage_.GetImages(), scratchImage_.GetImageCount(), scratchImage_.GetMetadata(), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, command_.mipLevel_, mipChain);

		// 生成に成功したら
		if (SUCCEEDED(hr)) {
			// イメージとメタデータをミップマップ版で置き換える
			scratchImage_ = std::move(mipChain);
			metaData_ = scratchImage_.GetMetadata();
		}

		// 圧縮形式に変換
		DirectX::ScratchImage converted;
		hr = DirectX::Compress(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metaData_, DXGI_FORMAT_BC7_UNORM_SRGB,
			DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_QUICK | DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_SRGB_OUT | DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_PARALLEL,
			1.f, converted);

		// 読み込んだデータをSRGBとして扱う
		metaData_.format = DirectX::MakeSRGB(metaData_.format);
		// 出力ファイルを設定する
		const std::wstring file_path = textureFileName_.GetFilePath(L".dds");
		// DDSファイルを書き出し
		hr = DirectX::SaveToDDSFile(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metaData_, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, file_path.c_str());
		assert(SUCCEEDED(hr) and "DDSファイルへの変換に失敗しました｡");

	}

	TextureConverter::TextureFileName TextureConverter::SeparateFilePath(const std::wstring_view &file_path) const
	{
		// 返すデータ
		TextureFileName result;
		// ファイル名の転送
		std::wstring filePath{ file_path };
		// 文字を変換
		std::replace(filePath.begin(), filePath.end(), '/', '\\');

		size_t pos1{};
		std::wstring exceptExt;
		// 区切り文字 '.'が出てくる最初の部分を探す
		pos1 = filePath.rfind('.');
		// 検索でヒットしたら
		if (pos1 != std::wstring::npos) {
			// 区切り文字の後ろを拡張子として読む
			result.fileExt_ = filePath.substr(pos1 + 1, filePath.size() - pos1 - 1);
			// それ以前を抜き出す
			exceptExt = filePath.substr(0, pos1);
		}
		// 拡張子がなかった場合
		else {
			result.fileExt_ = L"";
			exceptExt = filePath;
		}
		// 区切り文字'\\'が出てくる一番最後の部分を探す
		pos1 = exceptExt.rfind('\\');
		if (pos1 != std::wstring::npos) {
			// 区切り文字の前までをディレクトリパスとして保存
			result.directoryPath_ = exceptExt.substr(0, pos1 + 1);
			// 区切り文字以降をファイル名として保存
			result.fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		}
		// 生のフォルダだった場合
		else {
			result.directoryPath_ = L"";
			result.fileName_ = exceptExt;
		}

		return result;
	}

	TextureConverter::CommandLoader::CommandLoader(const std::span<char *> options)
	{
		for (auto itr = options.begin(); itr != options.end();) {
			const std::string_view option{ *itr };

			if (option == "-ml") {
				mipLevel_ = std::stoull(*++itr);
				continue;
			}

			itr++;
		}
	}
}