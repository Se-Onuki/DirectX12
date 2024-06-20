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
				//Equal(this->inputElementDescs_, that.inputElementDescs_) and
				//Equal(this->rasterizerDesc_, that.rasterizerDesc_) and
				//Equal(this->depthStencilDesc_, that.depthStencilDesc_) and
				this->rtvFormats_ == that.rtvFormats_ and
				//Equal(this->renderTarget_, that.renderTarget_) and
				true;
		}

	private:

		template<SoLib::IsRealType T>
			requires (requires(const T t) { { t == t } ->std::same_as<bool>; } and not SoLib::IsContainer<T>)
		static bool Equal(const T &l, const T &r) {

			return l == r;

		}
		template<SoLib::IsContainer Container>
		static bool Equal(const Container &l, const Container &r) {
			if (l.size() != r.size()) {
				return false;
			}
			auto lItr = l.begin();
			auto rItr = r.begin();
			for (size_t i = 0; i < l.size(); i++) {
				if (not Equal(*(lItr++), *(rItr++))) {
					return false;
				}
			}
			return true;
		}

		static bool Equal(const D3D12_INPUT_ELEMENT_DESC &l, const D3D12_INPUT_ELEMENT_DESC &r) {
			return
				l.SemanticIndex == r.SemanticIndex and
				l.Format == r.Format and
				l.InputSlot == r.InputSlot and
				l.AlignedByteOffset == r.AlignedByteOffset and
				l.InputSlotClass == r.InputSlotClass and
				l.InstanceDataStepRate == r.InstanceDataStepRate and
				// ポインタが一緒か、文字列が一致してたら true
				(l.SemanticName == r.SemanticName or std::strcmp(l.SemanticName, r.SemanticName) == 0) and
				true;

		}
		static bool Equal(const D3D12_RASTERIZER_DESC &l, const D3D12_RASTERIZER_DESC &r) {
			return
				l.FillMode == r.FillMode and
				l.CullMode == r.CullMode and
				l.FrontCounterClockwise == r.FrontCounterClockwise and
				l.DepthBias == r.DepthBias and
				l.DepthBiasClamp == r.DepthBiasClamp and
				l.SlopeScaledDepthBias == r.SlopeScaledDepthBias and
				l.DepthClipEnable == r.DepthClipEnable and
				l.MultisampleEnable == r.MultisampleEnable and
				l.AntialiasedLineEnable == r.AntialiasedLineEnable and
				l.ForcedSampleCount == r.ForcedSampleCount and
				l.ConservativeRaster == r.ConservativeRaster and
				true;

		}


		static bool Equal(const D3D12_DEPTH_STENCIL_DESC &l, const D3D12_DEPTH_STENCIL_DESC &r) {

			static constexpr std::tuple kMemPtr = { &D3D12_DEPTH_STENCIL_DESC::DepthEnable, &D3D12_DEPTH_STENCIL_DESC::DepthWriteMask, &D3D12_DEPTH_STENCIL_DESC::DepthFunc,&D3D12_DEPTH_STENCIL_DESC::StencilEnable,&D3D12_DEPTH_STENCIL_DESC::StencilReadMask,&D3D12_DEPTH_STENCIL_DESC::StencilWriteMask,&D3D12_DEPTH_STENCIL_DESC::FrontFace,&D3D12_DEPTH_STENCIL_DESC::BackFace, };
			return std::apply([&l, &r](const auto&... args_lhs) {

				return (Equal(l.*args_lhs, r.*args_lhs) && ...);
				},
				kMemPtr
			);
		}

		static bool Equal(const D3D12_DEPTH_STENCILOP_DESC &l, const D3D12_DEPTH_STENCILOP_DESC &r) {

			static constexpr std::tuple kMemPtr = { &D3D12_DEPTH_STENCILOP_DESC::StencilFailOp,&D3D12_DEPTH_STENCILOP_DESC::StencilDepthFailOp,&D3D12_DEPTH_STENCILOP_DESC::StencilPassOp, &D3D12_DEPTH_STENCILOP_DESC::StencilFunc, };
			return std::apply([&l, &r](const auto&... args_lhs) {

				return (Equal(l.*args_lhs, r.*args_lhs) && ...);
				},
				kMemPtr
			);
		}

		static bool Equal(const D3D12_RENDER_TARGET_BLEND_DESC &l, const D3D12_RENDER_TARGET_BLEND_DESC &r) {
			return std::memcmp(&l, &r, 36u) == 0 and l.RenderTargetWriteMask == r.RenderTargetWriteMask;
		}



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
