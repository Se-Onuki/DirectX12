#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <variant>

#include "../../../Utils/Text/StaticString.h"
#include "../../ResourceObject/ResourceObject.h"
#include <typeindex>
#include "../../../Utils/Types/TypeID.h"

namespace SolEngine {

	/// @brief ルートパラメータの保存クラス
	struct RootParameters
	{

		inline static constexpr uint32_t kParamStrSize_ = 5u;

		enum class BufferType : uint8_t {
			kCBV,
			kSRV,
			kUAV,

			kMaxCount
		};

		struct BufferData {
			BufferType type_;
			uint32_t index_;
			D3D12_SHADER_VISIBILITY shaderVisibility_ = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;


			BufferData() = default;
			BufferData(const BufferData &) = default;
			BufferData &operator=(const BufferData &) = default;

			BufferData(const char str[kParamStrSize_ - 1]) {
				switch (str[0])
				{
				case 'b':
					type_ = BufferType::kCBV;
					break;
				case 't':
					type_ = BufferType::kSRV;
					break;
				default:
					type_ = BufferType::kCBV;
					break;
				}
				// 文字列の変更
				index_ = static_cast<uint32_t>(str[1] - '0');

				shaderVisibility_ = ShaderVisibility(&str[2]);
			}

			template <SoLib::Text::ConstExprString CStr, SoLib::Text::StaticString<CStr> Str>
			consteval BufferData() {
				const char *const str = Str.c_str();

				switch (str[0])
				{
				case 'b':
					type_ = BufferType::kCBV;
					break;
				case 't':
					type_ = BufferType::kSRV;
					break;
				default:
					type_ = BufferType::kCBV;
					break;
				}
				// 文字列の変更
				index_ = static_cast<uint32_t>(str[1] - '0');

				shaderVisibility_ = ShaderVisibility(&str[2]);
			}

			bool operator==(const BufferData &) const = default;
		};

		//RootParameter(std::vector<std::pair<std::type_index, uint8_t>> &&data) : parameters_{ data } {}

		RootParameters() = default;
		~RootParameters() = default;

		bool operator==(const RootParameters &other) const = default;

		std::vector<BufferData> parameters_;
		RootParameters(const std::string &params) {
			for (uint32_t i = 0u; i < params.size(); i += kParamStrSize_) {
				const std::span<const char, kParamStrSize_> span{ &params[i], kParamStrSize_ };
				// 文字列からデータを構築する
				parameters_.push_back(BufferData{ span.data() });

			}

		}


		constexpr static D3D12_SHADER_VISIBILITY ShaderVisibility(const char shaderType[2]) {

			if (shaderType[1] == 'S') {
				switch (shaderType[0])
				{
				case 'V':
					return D3D12_SHADER_VISIBILITY_VERTEX;
				case 'P':
					return D3D12_SHADER_VISIBILITY_PIXEL;
				case 'H':
					return D3D12_SHADER_VISIBILITY_HULL;
				case 'D':
					return D3D12_SHADER_VISIBILITY_DOMAIN;
				case 'G':
					return D3D12_SHADER_VISIBILITY_GEOMETRY;
				case 'A':
					return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
				case 'M':
					return D3D12_SHADER_VISIBILITY_MESH;
				default:
					return D3D12_SHADER_VISIBILITY_ALL;
				}

			}
			return D3D12_SHADER_VISIBILITY_ALL;
		}

	};

	template<SoLib::IsRealType T>
	struct MonoParameter {
		RootParameters::BufferData bufferData_;
		std::variant<D3D12_GPU_VIRTUAL_ADDRESS(*)(const T &), D3D12_GPU_DESCRIPTOR_HANDLE(*)(const T &)> accesseFunc_;

		MonoParameter() = default;
		//MonoParameter(RootParameters::BufferData &&bufferData) : bufferData_(bufferData) {}
		MonoParameter(RootParameters::BufferData &&bufferData, std::variant<D3D12_GPU_VIRTUAL_ADDRESS(*)(const T &), D3D12_GPU_DESCRIPTOR_HANDLE(*)(const T &)> accesseFunc = {}) : bufferData_(bufferData), accesseFunc_(accesseFunc) {}
	};

	template<SoLib::IsRealType... Ts>
	struct RootParametersAccesser {

		std::tuple<MonoParameter<Ts>...> parameters_;

	};

	template<SoLib::IsRealType... Args>
	const RootParametersAccesser<Args...> MakeRootParametersAccesser(MonoParameter<Args>&&... args)
	{
		RootParametersAccesser<Args...> result;
		result.parameters_ = { MonoParameter<Args>{std::forward<MonoParameter<Args>>(args).bufferData_, std::forward<MonoParameter<Args>>(args).accesseFunc_}... };

		return result;
	}


}
template<SoLib::IsRealType T>
using SignParam = SolEngine::MonoParameter<T>;


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
	D3D12_STATIC_SAMPLER_DESC sampler_ = DefaultSampler();

	//std::string 

	SolEngine::RootParameters item_;

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