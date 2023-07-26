#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
//
//class PipelineState {
//
//	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
//
//	ComPtr<ID3D12PipelineState>graphicsPipelineState_ = nullptr;
//public:
//	PipelineState() = default;
//	~PipelineState() = default;
//
//	std::vector<D3D12_ROOT_PARAMETER>rootParameters_;
//	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
//	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;
//
//	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
//
//	void SetRootSignature();
//
//	void CreatePipeline();
//
//};
