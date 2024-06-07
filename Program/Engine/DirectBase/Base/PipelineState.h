#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <optional>

#include "RootSignature.h"
#include "Shader.h"

#include "../../../Utils/SoLib/SoLib_Traits.h"

class PipelineState {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	struct ShaderSet {
		SolEngine::ResourceObjectManager<Shader>::Handle vertex_;
		SolEngine::ResourceObjectManager<Shader>::Handle pixel_;

		void SetPipelineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC *const pipelineDesc);
	};

	PipelineState() = default;
	~PipelineState() = default;

	void Create(const RootSignature &rootSignature, const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc);

	void SetShader(const ShaderSet &shaderSet);

	template <SoLib::IsContainsType<D3D12_INPUT_ELEMENT_DESC> T>
	void SetInputElementDescs(const T &inputElementDescs);

private:

	const RootSignature *rootSignature_ = nullptr;

	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};

	ShaderSet shaderSet_;

};

template<SoLib::IsContainsType<D3D12_INPUT_ELEMENT_DESC> T>
inline void PipelineState::SetInputElementDescs(const T &inputElementDescs) {
	inputElementDescs_.assign(inputElementDescs.begin(), inputElementDescs.end());
}

class PipelineCreater {
public:

	PipelineState Output();


private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;

};
