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


class Shader {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// dxcCompilerを初期化
	static ComPtr<IDxcUtils>dxcUtils_;
	static ComPtr<IDxcCompiler3> dxcCompiler_;

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	static ComPtr<IDxcIncludeHandler> includeHandler_;

	static const std::wstring directoryPath_;

	ComPtr<IDxcBlob> shaderBlob_ = nullptr;

	std::wstring shaderPath_;

public:

	inline operator bool() noexcept;

	Shader() = default;
	~Shader() = default;

	static void StaticInit();

	static Shader Compile(const std::wstring &ShaderPath, const wchar_t *profile);

	inline IDxcBlob *const GetShaderBlob() const {
		return shaderBlob_.Get();
	}
	inline const std::wstring &GetShaderPath() const {
		return shaderPath_;
	}

	D3D12_SHADER_BYTECODE GetBytecode()const;
};

namespace std {
	template<>
	struct hash<Shader> {
		size_t operator()(const Shader &data) const {
			return std::hash<std::wstring>()(data.GetShaderPath());
		}
	};
}


Microsoft::WRL::ComPtr<IDxcBlob> const CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring &file_path,
	// Compilerに使用するProfile
	const wchar_t *profile,
	// 初期化して生成したものを3つ
	IDxcUtils *dxcUtils,
	IDxcCompiler3 *dxcCompiler,
	IDxcIncludeHandler *includeHandler
);