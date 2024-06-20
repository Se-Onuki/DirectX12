#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <optional>

#include "RootSignature.h"
#include "Shader.h"

#include "../../../Utils/SoLib/SoLib_Traits.h"

#include "../../ResourceObject/ResourceObject.h"
#include "../../Utils/Containers/ConstVector.h"

class PipelineState : SolEngine::IResourceObject {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	struct ShaderSet {
		SolEngine::ResourceObjectManager<Shader>::Handle vertex_;
		SolEngine::ResourceObjectManager<Shader>::Handle pixel_;
		SolEngine::ResourceObjectManager<Shader>::Handle domain_;
		SolEngine::ResourceObjectManager<Shader>::Handle hull_;
		SolEngine::ResourceObjectManager<Shader>::Handle geometry_;

		bool operator==(const ShaderSet &) const = default;

		void SetPipelineDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC *const pipelineDesc) const;
	};

	PipelineState() = default;
	~PipelineState() = default;

	//void Create(const RootSignature *rootSignature, const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc);

	//void SetShader(const ShaderSet &shaderSet);

	//template <SoLib::IsContainsType<D3D12_INPUT_ELEMENT_DESC> T>
	//void SetInputElementDescs(const T &inputElementDescs);

	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
private:

};
//
//template<SoLib::IsContainsType<D3D12_INPUT_ELEMENT_DESC> T>
//inline void PipelineState::SetInputElementDescs(const T &inputElementDescs) {
//	inputElementDescs_.assign(inputElementDescs.begin(), inputElementDescs.end());
//}

namespace SolEngine {

	template<>
	class ResourceSource<PipelineState> {
	public:

		ResourceObjectManager<RootSignature>::Handle rootSignature_;

		PipelineState::ShaderSet shaderSet_;

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;

		D3D12_RASTERIZER_DESC rasterizerDesc_;

		// DepthStencileStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

		SoLib::ConstVector<DXGI_FORMAT, 8u> rtvFormats_{ { DXGI_FORMAT{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB} } };

		std::vector<D3D12_RENDER_TARGET_BLEND_DESC> renderTarget_{ {.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL} };


		bool operator==(const ResourceSource<PipelineState> &that) const {
			return this->rootSignature_ == that.rootSignature_ and
				this->shaderSet_ == that.shaderSet_ and
				//this->inputElementDescs_ == that.inputElementDescs_ and
				//this->rasterizerDesc_ == that.rasterizerDesc_ and
				//this->depthStencilDesc_ == that.depthStencilDesc_ and
				this->rtvFormats_ == that.rtvFormats_ and
				//this->renderTarget_ == that.renderTarget_
				true;
		}

	private:


	};


	template<>
	class ResourceCreater<PipelineState> {
	public:
		std::unique_ptr<PipelineState> CreateObject(const ResourceSource<PipelineState> &source) const;
	};


}

template<>
struct std::hash<SolEngine::ResourceSource<PipelineState>> {
	size_t operator()(const SolEngine::ResourceSource<PipelineState> &data) const {
		std::string str;
		for (const auto &item : data.inputElementDescs_) {
			str += item.SemanticName + std::to_string(item.SemanticIndex) + std::to_string(item.Format) + std::to_string(item.InputSlot);
		}
		return std::hash<std::string>{}(str);
	}
};

class PipelineCreater {
public:

	PipelineState Output();


private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;

};
