#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>

#include "RootSignature.h"
#include "Shader.h"

class PipelineState {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	struct ShaderSet {
		std::unique_ptr<Shader> vertex = nullptr;
		std::unique_ptr<Shader> pixel = nullptr;
	};

	PipelineState() = default;
	~PipelineState() = default;

	void Create(const RootSignature &rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputElementDescs, const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc);

	void SetShader(const ShaderSet &shader);



private:

	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};

};
