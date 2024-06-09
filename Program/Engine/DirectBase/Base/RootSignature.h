#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#include "../../../Utils/Text/StaticString.h"
#include "../../ResourceObject/ResourceObject.h"
#include <typeindex>
#include "../../../Utils/Types/TypeID.h"

namespace SolEngine {

	/// @brief ルートパラメータの保存クラス
	struct RootParameters
	{
		enum class BufferType {
			kCBV,
			kSRV,
			kUAV,
		};

		struct BufferData {
			BufferType type_;
			uint32_t index_;
			D3D12_SHADER_VISIBILITY shaderVisibility_;

			bool operator==(const BufferData &) const = default;
		};

		RootParameters(std::vector<std::pair<std::type_index, uint8_t>> &&data) : parameters_{ data } {}

		RootParameters() = default;
		~RootParameters() = default;

		bool operator==(const RootParameters &other) const = default;

		template<SoLib::IsRealType T>
		static std::pair<std::type_index, uint8_t> MakePair(uint8_t p) {
			return std::pair<std::type_index, uint8_t>{ typeid(T), p};
		}

		std::vector<std::pair<std::type_index, uint8_t>> parameters_;

	};

	//template <SoLib::IsRealType... Ts>
	//struct RootParameters : IRootParameters {
	//public:
	//	RootParameters() = default;
	//	bool operator==(const RootParameters &other) const = default;
	//
	//	uint32_t GetIndex(const std::type_index &typeId) const override {
	//
	//	}
	//
	//private:
	//
	//	std::tuple<std::pair<Ts, char>...> types_;
	//
	//};
}


class RootSignature final : public SolEngine::IResourceObject {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	RootSignature() = default;
	~RootSignature() = default;

	void Init(const D3D12_ROOT_PARAMETER *const rootParameter, const size_t parameterSize);

	auto *const Get() const { return rootSignature_.Get(); }
	void Set(ComPtr<ID3D12RootSignature> &&rootSignature) { rootSignature_ = rootSignature; }

	//SolEngine::RootParameters rootParameters_;

private:

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

};

template <>
class SolEngine::ResourceSource<RootSignature> {
public:
	std::vector<D3D12_ROOT_PARAMETER> rootParameter_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> sampler_;

	//std::string 

	std::vector<SolEngine::RootParameters::BufferData> item_;

	//std::vector<RootParameters> types_;
	//SolEngine::RootParameters types_;

	static D3D12_STATIC_SAMPLER_DESC DefaultSampler();

	bool operator==(const SolEngine::ResourceSource<RootSignature> &r) const {
		return item_ == r.item_;
	}
};

template <>
class SolEngine::ResourceCreater<RootSignature> {
public:

	std::unique_ptr<RootSignature> CreateObject(const SolEngine::ResourceSource<RootSignature> &source) const;

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
			std::string result;
			for (const auto &item : data.item_) {
				result += std::to_string(static_cast<uint32_t>(item.type_)) + '/' + std::to_string(item.index_) + '/' + std::to_string(item.shaderVisibility_) + '/';
			}
			return std::hash<std::string>{}(result);
		}
	};
}