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
	struct RootParameter
	{
		enum class BufferType : uint8_t {
			kCBV,
			kSRV,
			kUAV,

			kMaxCount
		};

		struct BufferData {
			BufferType type_;
			D3D12_SHADER_VISIBILITY shaderVisibility_ = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

			bool operator==(const BufferData &) const = default;
		};

		//RootParameter(std::vector<std::pair<std::type_index, uint8_t>> &&data) : parameters_{ data } {}

		RootParameter() = default;
		~RootParameter() = default;

		bool operator==(const RootParameter &other) const = default;

		/*template<SoLib::IsRealType T, uint8_t type>
		static std::pair<std::type_index, uint8_t> MakePair() {
			return std::pair<std::type_index, uint8_t>{ typeid(T), type};
		}*/

		inline static constexpr uint32_t kParamSize_ = 2u;

		std::vector<BufferData> parameters_;
		RootParameter(const std::string &params) {
			for (uint32_t i = 0u; i < params.size(); i += kParamSize_) {
				const std::span<const char, kParamSize_> span{ &params[i], kParamSize_ };

				BufferData &data = parameters_.emplace_back();
				switch (span[0])
				{
				case 'b':
					data.type_ = BufferType::kCBV;
					break;
				case 't':
					data.type_ = BufferType::kSRV;
					break;
				default:
					break;
				}

			}
		}


	};

	template <SoLib::IsRealType... Ts>
	struct RootParameters {

		RootParameters() : parameters_{ RootParameter::BufferData{.type_ = CheckType<Ts>()}... } {}

		const std::array<RootParameter::BufferData, sizeof...(Ts)> parameters_;

	private:

		using tuple = std::tuple<Ts...>;

		template<SoLib::IsRealType T>
		inline static constexpr RootParameter::BufferType CheckType() {
			if constexpr (requires(T a) {
				{ a.GetView() } -> std::same_as<const D3D12_CONSTANT_BUFFER_VIEW_DESC &>;
			}) {
				return RootParameter::BufferType::kCBV;
			}
			else {
				return RootParameter::BufferType::kSRV;
			}
		}


	};

}


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

template <>
class SolEngine::ResourceSource<RootSignature> {
public:
	D3D12_STATIC_SAMPLER_DESC sampler_;

	//std::string 

	SolEngine::RootParameter item_;

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
			for (const auto &item : data.item_.parameters_) {
				result += std::to_string(static_cast<uint32_t>(item.type_)) + '/' + std::to_string(item.shaderVisibility_) + '/';
			}
			return std::hash<std::string>{}(result);
		}
	};
}