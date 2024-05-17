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

private:

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

};

class RootSignatureCreater {

};