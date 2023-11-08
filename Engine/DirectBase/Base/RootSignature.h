#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

class RootSignature final {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	RootSignature() = default;
	~RootSignature() = default;

	void Create(const D3D12_ROOT_PARAMETER *const rootParameter, const size_t parameterSize);

	auto *const Get() const { return rootSignature_.Get(); }

private:

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

};