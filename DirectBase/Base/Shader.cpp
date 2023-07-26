#include "Shader.h"

Microsoft::WRL::ComPtr<IDxcUtils>Shader::dxcUtils_;
Microsoft::WRL::ComPtr<IDxcCompiler3> Shader::dxcCompiler_;

Microsoft::WRL::ComPtr<IDxcIncludeHandler> Shader::includeHandler_;

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

void Shader::Compile(const std::wstring &ShaderPath, const wchar_t *profile)
{
	shaderBlob_ = CompileShader(ShaderPath, profile, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(shaderBlob_ != nullptr);
}
