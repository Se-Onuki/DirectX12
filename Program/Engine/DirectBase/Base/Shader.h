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
#include "../../ResourceObject/ResourceObjectManager.h"

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
class SolEngine::ResourceSource<Shader> {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	using Resource = Shader;

	std::wstring name_;
	std::wstring profile_;

	std::string ToStr() const {
		return ConvertString(name_) + '/' + ConvertString(profile_);
	}

	bool operator==(const SolEngine::ResourceSource<Shader> &other) const = default;
};


template <>
class SolEngine::ResourceCreater<Shader, SolEngine::ResourceSource<Shader>> {
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ResourceCreater() {

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

	inline static const std::wstring directoryPath_ = L"resources/shader/";

	std::unique_ptr<Shader> Compile(const std::wstring &shaderPath, const wchar_t *profile) const;

	std::unique_ptr<Shader> CreateObject(const SolEngine::ResourceSource<Shader> &source) const { return std::move(Compile(source.name_, source.profile_.c_str())); }


private:
	// dxcCompilerを初期化
	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	ComPtr<IDxcIncludeHandler> includeHandler_;

};


using ShaderSource = SolEngine::ResourceSource<Shader>;

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

namespace SolEngine {
	using ShaderManager = ResourceObjectManager<Shader, ResourceSource<Shader>, ResourceCreater<Shader, SolEngine::ResourceSource<Shader>>>;
}