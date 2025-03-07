/// @file Shader.h
/// @brief シェーダの実装
/// @author ONUKI seiya
#pragma once

#include <cassert>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "../String/String.h"
#include "DirectXCommon.h"
#include "EngineObject.h"
#include <format>
#include <string>
#include <wrl.h>

/// @class Shader
/// @brief シェーダの実装
class Shader : public SolEngine::IResourceObject
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDxcBlob> shaderBlob_ = nullptr;

	std::wstring shaderPath_;

public:
	inline operator bool() noexcept;

	Shader() = default;
	~Shader() = default;

	inline IDxcBlob *const GetShaderBlob() const
	{
		return shaderBlob_.Get();
	}

	void SetIDxcBlob(ComPtr<IDxcBlob> &&shaderBlob) { shaderBlob_ = std::move(shaderBlob); }
	void SetShaderPath(const std::wstring &str) { shaderPath_ = str; }

	/// @fn const std::wstring &GetShaderPath(void)
	/// @brief シェーダのファイルパスを取得
	/// @return ファイルパスの文字列
	inline const std::wstring &GetShaderPath() const
	{
		return shaderPath_;
	}
	/// @fn D3D12_SHADER_BYTECODE GetBytecode(void)
	/// @brief シェーダのバイトコードの取得
	/// @return バイトコード
	D3D12_SHADER_BYTECODE GetBytecode() const;
};

template <>
class SolEngine::ResourceSource<Shader>
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	using Resource = Shader;

	std::wstring name_;
	std::wstring profile_;

	/// @fn ToStr(void)
	/// @brief 文字列化する
	/// @return 文字列
	std::string ToStr() const
	{
		return ConvertString(name_) + '/' + ConvertString(profile_);
	}

	bool operator==(const SolEngine::ResourceSource<Shader> &other) const = default;
};

template <>
class SolEngine::ResourceCreater<Shader, SolEngine::ResourceSource<Shader>>
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ResourceCreater()
	{

		HRESULT hr = S_FALSE;
		// dxcCompilerを初期化
		dxcUtils_ = nullptr;
		dxcCompiler_ = nullptr;
		// dxcUtilsを初期化
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
		assert(SUCCEEDED(hr));
		// dxcCompilerを初期化
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
		assert(SUCCEEDED(hr));

		// 現時点でincludeはしないが、includeに対応するための設定を行っておく
		includeHandler_ = nullptr;
		// includeHandlerを初期化
		hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
		assert(SUCCEEDED(hr));
	}

	inline static const std::wstring directoryPath_ = L"resources/shader/";

	/// @brief シェーダをコンパイルする
	/// @param[in] shaderPath シェーダのファイルパス
	/// @param[in] profile シェーダのプロファイル
	/// @return コンパイルされたシェーダ
	std::unique_ptr<Shader> Compile(const std::wstring &shaderPath, const wchar_t *profile) const;

	/// @brief オブジェクトの生成
	/// @param[in] source ソースデータ
	/// @return 生成されたオブジェクト
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

	template <>
	struct hash<Shader>
	{
		size_t operator()(const Shader &data) const
		{
			return std::hash<std::wstring>()(data.GetShaderPath());
		}
	};

	template <>
	struct hash<ShaderSource>
	{
		size_t operator()(const ShaderSource &data) const
		{
			return std::hash<std::wstring>()(data.name_ + data.profile_);
		}
	};
}

/// @brief シェーダをコンパイルする
/// @param[in] file_path コンパイル対象のhlslファイルへのパス
/// @param[in] profile コンパイルに使用するProfile
/// @param[in] dxcUtils 初期化して生成したものを3つ
/// @param[in] dxcCompiler 初期化して生成したものを3つ
/// @param[in] includeHandler 初期化して生成したものを3つ
Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring &file_path,
	// Compilerに使用するProfile
	const wchar_t *profile,
	// 初期化して生成したものを3つ
	IDxcUtils *dxcUtils,
	IDxcCompiler3 *dxcCompiler,
	IDxcIncludeHandler *includeHandler);

namespace SolEngine {
	using ShaderManager = ResourceObjectManager<Shader, ResourceSource<Shader>, ResourceCreater<Shader, SolEngine::ResourceSource<Shader>>>;
}