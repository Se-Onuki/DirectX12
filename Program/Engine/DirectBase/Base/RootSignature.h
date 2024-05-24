#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#include "../../ResourceObject/ResourceObject.h"

class RootSignature final : public SolEngine::IResourceObject {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	RootSignature() = default;
	~RootSignature() = default;

	void Init(const D3D12_ROOT_PARAMETER *const rootParameter, const size_t parameterSize);

	auto *const Get() const { return rootSignature_.Get(); }
	void Set(ComPtr<ID3D12RootSignature> &&rootSignature) { rootSignature_ = rootSignature; }

private:

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

};

struct IRootParameter
{
	char16_t paramType_;

	//virtual ~IRootParameter() = 0;
};
template <SoLib::IsRealType T>
struct RootParameterTexture {
	
};

template <>
class SolEngine::ResourceSource<RootSignature> {
public:
	std::vector<D3D12_ROOT_PARAMETER> rootParameter_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> sampler_;

	//std::string 

	static D3D12_STATIC_SAMPLER_DESC DefaultSampler();

	bool operator==(const SolEngine::ResourceSource<RootSignature> &r) const {
		return rootParameter_.size() == r.rootParameter_.size();

	}
};

template <>
class SolEngine::ResourceCreater<RootSignature> {
public:

	std::unique_ptr<RootSignature> CreateObject(const SolEngine::ResourceSource<RootSignature> &source);

};

inline D3D12_STATIC_SAMPLER_DESC SolEngine::ResourceSource<RootSignature>::DefaultSampler()
{
	return D3D12_STATIC_SAMPLER_DESC{
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,			// バイナリフィルタ
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,		// 0~1の範囲外をリピート
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,		// 比較しない
		.MaxLOD = D3D12_FLOAT32_MAX,						// ありったけのMipMapを使う
		.ShaderRegister = 0,								// レジスタ番号 0を使う
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,	// PixelShaderで使う
	};
}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<RootSignature>> {
		size_t operator()(const SolEngine::ResourceSource<RootSignature> &data) const {
			// 一旦0を返すことにする
			return data.rootParameter_.size() + data.sampler_.size() /*std::hash<std::wstring>()(data.rootParameter_)*/;
		}
	};
}