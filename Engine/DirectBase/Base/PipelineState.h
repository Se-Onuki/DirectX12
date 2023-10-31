#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>

#include "RootSignature.h"
#include "Shader.h"

class PipelineState {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	PipelineState() = default;
	~PipelineState() = default;

	void Create(const RootSignature &rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputElementDescs, const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc);

private:

	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	Shader vertexShader_;
	Shader pixelShader_;

};
