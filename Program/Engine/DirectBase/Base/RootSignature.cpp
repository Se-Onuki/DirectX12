#include "RootSignature.h"
#include "DirectXCommon.h"

void RootSignature::Init(const D3D12_ROOT_PARAMETER *const rootParameter, const size_t parameterSize) {

	HRESULT hr = S_FALSE;
	auto *const device = DirectXCommon::GetInstance()->GetDevice();

#pragma region RootSigneture(ルートシグネチャ)

	// RootSignature生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region RootParameter

	descriptionRootSignature.pParameters = rootParameter;		// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = static_cast<UINT>(parameterSize);		// 配列の長さ

#pragma endregion

#pragma region Samplerの設定

	D3D12_STATIC_SAMPLER_DESC staticSamplers[] = {
	{
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,			// バイナリフィルタ
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,		// 0~1の範囲外をリピート
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,		// 比較しない
		.MaxLOD = D3D12_FLOAT32_MAX,						// ありったけのMipMapを使う
		.ShaderRegister = 0,								// レジスタ番号 0を使う
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,	// PixelShaderで使う
	}
	};
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

#pragma endregion

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に作成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
	assert(SUCCEEDED(hr));

#pragma endregion

}


std::unique_ptr<RootSignature> SolEngine::ResourceCreater<RootSignature>::CreateObject(const SolEngine::ResourceSource<RootSignature> &source) const {
	HRESULT hr = S_FALSE;
	auto *const device = DirectXCommon::GetInstance()->GetDevice();

	std::unique_ptr<RootSignature> result = std::make_unique<RootSignature>();

#pragma region RootSigneture(ルートシグネチャ)

	// RootSignature生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	std::vector<D3D12_ROOT_PARAMETER> parameters;
	parameters.reserve(source.item_.parameters_.size());
	// 一時保存用のデータ
	std::list<D3D12_DESCRIPTOR_RANGE> descRanges{};

	for (const auto &item : source.item_.parameters_) {

		D3D12_ROOT_PARAMETER parameter{};
		parameter.ShaderVisibility = item.shaderVisibility_;

		switch (item.type_)
		{
		case RootParameter::BufferType::kCBV:
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			parameter.Descriptor.ShaderRegister = item.index_;
			break;
		case RootParameter::BufferType::kSRV:
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			// DescriptorRangeの設定
			D3D12_DESCRIPTOR_RANGE &descriptorRange = descRanges.emplace_back();
			descriptorRange.BaseShaderRegister = item.index_;
			descriptorRange.NumDescriptors = 1;                                                       // 数は1つ
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

			parameter.DescriptorTable.pDescriptorRanges = &descriptorRange;
			parameter.DescriptorTable.NumDescriptorRanges = 1;

			break;
		/*case RootParameters::BufferType::kUAV:
			break;
		default:
			break;*/
		}

		parameters.push_back(parameter);
	}

#pragma region RootParameter

	descriptionRootSignature.pParameters = parameters.data();		// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = static_cast<UINT>(parameters.size());		// 配列の長さ

#pragma endregion

#pragma region Samplerの設定

	descriptionRootSignature.pStaticSamplers = &source.sampler_;
	descriptionRootSignature.NumStaticSamplers = 1;

#pragma endregion

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSigunature;

	// バイナリを元に作成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSigunature.GetAddressOf()));
	assert(SUCCEEDED(hr));

	result->Set(std::move(rootSigunature));

#pragma endregion

	return std::move(result);
}