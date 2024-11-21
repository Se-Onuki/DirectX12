#include <iostream>
#include <cstdint>
#include <span>
#include <assert.h>
#include "TexConverter/TextureConverter.h"

#include <combaseapi.h>	// COM系

enum class Argument {
	kAppPath,	// アプリケーションのパス
	kFilePath,	// ファイルパス

	kNUM	// 値の総数
};

int32_t main(int32_t argc, char *argv[]) {

	// COMの初期化処理
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr) and "初期化に失敗しました｡");

	// 引数のリスト
	const std::span<char *> args{ argv, static_cast<size_t>(argc) };
	// もし範囲外ならエラーを吐く
	assert(args.size() >= static_cast<size_t>(Argument::kNUM) and "引数が一致しません｡");

	// テクスチャの変換クラス
	SoLib::TextureConverter textureConverter{};

	// テクスチャへのファイルパスを変換関数に渡す
	textureConverter.ConvertTextureWIC2DDS(args[static_cast<uint32_t>(Argument::kFilePath)]);

	std::system("pause");

	// 終了処理
	CoUninitialize();

	return 0;

}