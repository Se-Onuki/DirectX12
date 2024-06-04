#include "Shader.h"
inline Shader::operator bool() noexcept {
	return shaderBlob_ != nullptr;
}

D3D12_SHADER_BYTECODE Shader::GetBytecode() const {
	return D3D12_SHADER_BYTECODE{ shaderBlob_->GetBufferPointer(),shaderBlob_->GetBufferSize() };
}

std::unique_ptr<Shader> SolEngine::ResourceCreater<Shader>::Compile(const std::wstring &shaderPath, const wchar_t *profile) const
{
	std::unique_ptr<Shader> result = std::make_unique<Shader>();
	result->SetIDxcBlob(std::move(CompileShader(directoryPath_ + shaderPath, profile, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get())));
	result->SetShaderPath(shaderPath);
	assert(result->GetShaderBlob() and "シェーダーを読み込めませんでした");
	return std::move(result);
}

Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
	const std::wstring &file_path,
	const wchar_t *profile,
	IDxcUtils *dxcUtils,
	IDxcCompiler3 *dxcCompiler,
	IDxcIncludeHandler *includeHandler
) {

#pragma region 1. hlslファイルを読む

	// これからシェーダをコンパイルする旨をログに出す
	DirectXCommon::Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", file_path, profile)));
	// hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding>shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(file_path.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

#pragma endregion

#pragma region 2. Compileする

	LPCWSTR arguments[] = {
		file_path.c_str(),			// コンパイル対象のhlslファイル名
		L"-E", L"main",				// エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile,				// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",						// 最適化を外しておく
		L"-Zpr",					// メモリレイアウトは行優先
	};

	// 実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		// 読み込んだファイル
		arguments,					// コンパイルオプション
		_countof(arguments),		// コンパイルオプションの数
		includeHandler,				// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	// コンパイル結果
	);

	// コンパイルエラーではなくdxcが起動できないなど致命的な状態
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 3. 警告・エラーがでていないか確認する

	// 警告・エラーが出てたらログに出して止める
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		DirectXCommon::Log(shaderError->GetStringPointer());
		// 警告・エラー、ダメゼッタイ
		assert(false);
	}

#pragma endregion

#pragma region 4. Compile結果を受け取って返す

	// コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	DirectXCommon::Log(ConvertString(std::format(L"Compile Succeeded, path: {}, profile: {}\n", file_path, profile)));
	//実行用のバイナリを返却
	return shaderBlob;

#pragma endregion


}