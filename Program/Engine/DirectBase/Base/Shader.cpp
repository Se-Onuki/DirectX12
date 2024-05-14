#include "Shader.h"

Microsoft::WRL::ComPtr<IDxcUtils>Shader::dxcUtils_;
Microsoft::WRL::ComPtr<IDxcCompiler3> Shader::dxcCompiler_;

Microsoft::WRL::ComPtr<IDxcIncludeHandler> Shader::includeHandler_;
const std::wstring Shader::directoryPath_ = L"resources/shader/";

inline Shader::operator bool() noexcept {
	return shaderBlob_ != nullptr;
}

void Shader::StaticInit()
{
	HRESULT hr = S_FALSE;
	// dxcCompilerを初期化
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));


}

D3D12_SHADER_BYTECODE Shader::GetBytecode() const {
	return D3D12_SHADER_BYTECODE{ shaderBlob_->GetBufferPointer(),shaderBlob_->GetBufferSize() };
}

Shader Shader::Compile(const std::wstring &ShaderPath, const wchar_t *profile)
{
	Shader result;
	result.shaderBlob_ = CompileShader(directoryPath_ + ShaderPath, profile, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	result.shaderPath_ = ShaderPath;
	assert(result.shaderBlob_ and "シェーダーを読み込めませんでした");
	return result;
}

Microsoft::WRL::ComPtr<IDxcBlob> const CompileShader(
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