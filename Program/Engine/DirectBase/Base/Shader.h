#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <wrl.h>
#include <string>
#include "DirectXCommon.h"
#include "../String/String.h"
#include <format>
#include "EngineObject.h"
#include "../../ResourceObject/ResourceObject.h"

class Shader : public SolEngine::IResourceObject {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDxcBlob> shaderBlob_ = nullptr;

	std::wstring shaderPath_;

public:

	inline operator bool() noexcept;

	Shader() = default;
	~Shader() = default;

	inline IDxcBlob *const GetShaderBlob() const {
		return shaderBlob_.Get();
	}

	void SetIDxcBlob(ComPtr<IDxcBlob> &&shaderBlob) { shaderBlob_ = std::move(shaderBlob); }
	void SetShaderPath(const std::wstring &str) { shaderPath_ = str; }

	inline const std::wstring &GetShaderPath() const {
		return shaderPath_;
	}

	D3D12_SHADER_BYTECODE GetBytecode()const;
};

template<>
class SolEngine::IResourceSource<Shader> {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// dxcCompilerを初期化
	static ComPtr<IDxcUtils>dxcUtils_;
	static ComPtr<IDxcCompiler3> dxcCompiler_;

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	static ComPtr<IDxcIncludeHandler> includeHandler_;


public:

	static void StaticInit();


	std::wstring name_;
	std::wstring profile_;

	inline static const std::wstring directoryPath_ = L"resources/shader/";

	static Shader Compile(const std::wstring &shaderPath, const wchar_t *profile);
	std::unique_ptr<Shader> CreateObject() const { return std::make_unique<Shader>(Compile(name_, profile_.c_str())); }


	bool operator==(const SolEngine::IResourceSource<Shader> &other) const = default;
};


using ShaderSource = SolEngine::IResourceSource<Shader>;


inline void ShaderSource::StaticInit()
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


namespace std {

	template<>
	struct hash<Shader> {
		size_t operator()(const Shader &data) const {
			return std::hash<std::wstring>()(data.GetShaderPath());
		}
	};

	template<>
	struct hash<ShaderSource> {
		size_t operator()(const ShaderSource &data) const {
			return std::hash<std::wstring>()(data.name_ + data.profile_);
		}
	};
}


Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring &file_path,
	// Compilerに使用するProfile
	const wchar_t *profile,
	// 初期化して生成したものを3つ
	IDxcUtils *dxcUtils,
	IDxcCompiler3 *dxcCompiler,
	IDxcIncludeHandler *includeHandler
);