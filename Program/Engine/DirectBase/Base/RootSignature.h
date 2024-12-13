/// @file RootSignature.h
/// @brief ルートシグネチャ
/// @author ONUKI seiya
#pragma once
#include <d3d12.h>
#include <variant>
#include <vector>
#include <wrl.h>

#include "../../Engine/Utils/Text/StaticString.h"
#include "../../Engine/Utils/Types/TypeID.h"
#include "../../ResourceObject/ResourceObject.h"
#include <typeindex>

namespace SolEngine {

	/// @struct RootParameters
	/// @brief ルートパラメータの保存クラス
	struct RootParameters
	{
		/// @brief パラメータ一つの文字列のサイズ
		inline static constexpr uint32_t kParamStrSize_ = 5u;

		/// @enum BufferType
		/// @brief バッファの種類
		enum class BufferType : uint8_t
		{
			kCBV,		  // コンスタントバッファビュー
			kB32,		  // 32bitバッファ
			kStrBuff,	  // ストラクチャードバッファ
			kTextureView, // テクスチャビュー
			kUAV,		  // 順序無しアクセスビュー

			kMaxCount // 最大数
		};

		/// @struct BufferData
		/// @brief 転送するデータ
		struct BufferData
		{
			BufferType type_;																				  // バッファの種類
			uint32_t index_;																				  // インデックス
			D3D12_SHADER_VISIBILITY shaderVisibility_ = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL; // シェーダからの可視性

			BufferData() = default;
			BufferData(const BufferData &) = default;
			BufferData &operator=(const BufferData &) = default;

			/// @fn BufferData(const char[kParamStrSize_ - 1])
			/// @brief コンストラクタ
			/// @param[in] str パラメータの文字列
			BufferData(const char str[kParamStrSize_ - 1])
			{
				// 0番目はバッファの種類
				switch (str[0]) {
				case 'b':
					type_ = BufferType::kCBV;
					break;
				case 't':
					type_ = BufferType::kTextureView;
					break;
				case 'u':
					type_ = BufferType::kUAV;
					break;
				default:
					type_ = BufferType::kCBV;
					break;
				}

				// indexの番号
				index_ = static_cast<uint32_t>(str[1] - '0');
				// 可視性を指定する
				shaderVisibility_ = ShaderVisibility(&str[2]);
			}

			/// @fn BufferData(const char (&)[Size])
			/// @brief コンストラクタ
			/// @param[in] str 文字列
			template <size_t Size>
			consteval BufferData(const char (&str)[Size])
			{
				// 0番目はバッファの種類
				switch (str[0]) {
				case 'b':
					type_ = BufferType::kCBV;
					break;
				case 't':
					type_ = BufferType::kTextureView;
					break;
				case 'u':
					type_ = BufferType::kUAV;
					break;
				default:
					type_ = BufferType::kCBV;
					break;
				}
				// indexの番号
				index_ = static_cast<uint32_t>(str[1] - '0');
				// 可視性を指定する
				shaderVisibility_ = ShaderVisibility(&str[2]);
			}

			bool operator==(const BufferData &) const = default;
		};

		RootParameters() = default;
		~RootParameters() = default;

		bool operator==(const RootParameters &other) const = default;

		std::vector<BufferData> parameters_;

		/// @fn RootParameters(const std::string &)
		/// @brief コンストラクタ
		/// @param[in] params パラメータの文字列
		RootParameters(const std::string &params)
		{
			// 文字列の数だけループを回す｡ 5文字単位
			for (uint32_t i = 0u; i < params.size(); i += kParamStrSize_) {
				// 5文字単位の文字列を取り出す
				const std::span<const char, kParamStrSize_> span{&params[i], kParamStrSize_};
				// 文字列からデータを構築する
				parameters_.push_back(BufferData{span.data()});
			}
		}

		/// @brief 可視性を返す
		/// @param[in] shaderType 可視性の文字列
		/// @return 可視性
		constexpr static D3D12_SHADER_VISIBILITY ShaderVisibility(const char shaderType[2])
		{

			if (shaderType[1] == 'S') {
				switch (shaderType[0]) {
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

	/// @struct MonoParameter
	/// @brief 1つのパラメータを表す
	template <SoLib::IsRealType T>
	struct MonoParameter
	{
		RootParameters::BufferData bufferData_;

		static constexpr bool kIsHasVAddress_ = requires(T t) { { t.GetGPUVirtualAddress() } -> std::same_as<D3D12_GPU_VIRTUAL_ADDRESS>; };

		MonoParameter() = default;
		MonoParameter(RootParameters::BufferData &&bufferData) : bufferData_(bufferData) {}
	};

	/// @fn RootParametersAccesser
	/// @brief ルートパラメータのアクセッサー
	template <SoLib::IsRealType... Ts>
	struct RootParametersAccesser
	{

		std::tuple<MonoParameter<Ts>...> parameters_;

		/// @fn uint32_t GetIndex<T>(void)
		/// @brief その型のパラメータのインデックスを返す
		template <SoLib::IsRealType T>
		static constexpr uint32_t GetIndex() { return SoLib::IndexOfV<MonoParameter<T>, decltype(parameters_)>; }

		/// @fn RootParameters MakeRootParameters(void)
		/// @brief ルートパラメータを作成する
		/// @return ルートパラメータ
		RootParameters MakeRootParameters() const
		{
			RootParameters result;

			result.parameters_.reserve(sizeof...(Ts));
			result.parameters_ = {{std::get<MonoParameter<Ts>>(parameters_).bufferData_}...};

			return result;
		}
	};

	/// @fn RootParametersAccesser<Args...> MakeRootParametersAccesser(MonoParameter<Args> &&...)
	/// @brief ルートパラメータのアクセッサーを作成する
	/// @param ...args 型指定
	/// @return ルートパラメータ
	template <SoLib::IsRealType... Args>
	const RootParametersAccesser<Args...> MakeRootParametersAccesser(MonoParameter<Args> &&...args)
	{
		RootParametersAccesser<Args...> result;
		result.parameters_ = {MonoParameter<Args>{std::forward<MonoParameter<Args>>(args).bufferData_}...};

		return result;
	}

}
template <SoLib::IsRealType T>
using SignParam = SolEngine::MonoParameter<T>;

/// @class RootSignature
/// @brief ルートシグネチャ
class RootSignature final : public SolEngine::IResourceObject
{

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	RootSignature() = default;
	~RootSignature() = default;

	/// @fn void Init(const D3D12_ROOT_PARAMETER *const, const size_t)
	/// @brief 初期化
	/// @param[in] rootParameter ルートパラメータの配列
	/// @param[in] parameterSize 配列の長さ
	void Init(const D3D12_ROOT_PARAMETER *const rootParameter, const size_t parameterSize);

	/// @fn ID3D12RootSignature *const Get(void)
	/// @brief ルートシグネチャの取得
	/// @return ルートシグネチャ
	auto *const Get() const { return rootSignature_.Get(); }
	/// @fn void Set(ComPtr<ID3D12RootSignature> &&)
	/// @brief ルートシグネチャの設定
	/// @param[in] rootSignature ルートシグネチャ
	void Set(ComPtr<ID3D12RootSignature> &&rootSignature) { rootSignature_ = rootSignature; }

private:
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
};


/// @class SolEngine::ResourceSource<RootSignature>
/// @brief ルートシグネチャのソースデータ
template <>
class SolEngine::ResourceSource<RootSignature>
{
public:
	std::optional<D3D12_STATIC_SAMPLER_DESC> sampler_;

	// std::string

	SolEngine::RootParameters item_;

	// std::vector<RootParameters> types_;
	// SolEngine::RootParameters types_;

	/// @fn static D3D12_STATIC_SAMPLER_DESC DefaultSampler(void)
	/// @brief デフォルトのサンプラーを返す
	/// @return デフォルトのサンプラー
	static D3D12_STATIC_SAMPLER_DESC DefaultSampler();

	bool operator==(const SolEngine::ResourceSource<RootSignature> &r) const
	{
		return item_ == r.item_;
	}
};

template <>
class SolEngine::ResourceCreater<RootSignature>
{
public:
	/// @fn std::unique_ptr<RootSignature> CreateObject(const SolEngine::ResourceSource<RootSignature> &) const
	/// @brief 生成関数
	/// @param[in] source 生成ソース
	/// @return 生成したオブジェクト
	std::unique_ptr<RootSignature> CreateObject(const SolEngine::ResourceSource<RootSignature> &source) const;
};

inline D3D12_STATIC_SAMPLER_DESC SolEngine::ResourceSource<RootSignature>::DefaultSampler()
{
	return D3D12_STATIC_SAMPLER_DESC{
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,	 // バイナリフィルタ
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP, // 0~1の範囲外をリピート
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,	   // 比較しない
		.MaxLOD = D3D12_FLOAT32_MAX,					   // ありったけのMipMapを使う
		.ShaderRegister = 0,							   // レジスタ番号 0を使う
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL, // PixelShaderで使う
	};
}

namespace std {

	template <>
	struct hash<SolEngine::ResourceSource<RootSignature>>
	{
		size_t operator()(const SolEngine::ResourceSource<RootSignature> &data) const
		{
			std::string result;
			for (const auto &item : data.item_.parameters_) {
				result += std::to_string(static_cast<uint32_t>(item.type_)) + '/' + std::to_string(item.shaderVisibility_) + '/';
			}
			return std::hash<std::string>{}(result);
		}
	};
}