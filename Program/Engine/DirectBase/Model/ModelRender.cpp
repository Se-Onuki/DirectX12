/// @file ModelRender.cpp
/// @brief モデルの描画クラス
/// @author ONUKI seiya
#include "ModelRender.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"

namespace SolEngine {

	void SolEngine::ModelRender::Init()
	{
		dxCommon_ = ::DirectXCommon::GetInstance();
		commandList_ = dxCommon_->GetCommandList();

		ResourceObjectManager<RootSignature> *const rootSignatureManager = ResourceObjectManager<RootSignature>::GetInstance();

		const auto accesser = MakeRootParametersAccesser(SignParam<CBuffer<SoLib::TransformMatrix>>{"b0VS"});

		rootSignature_ = rootSignatureManager->Load({ .sampler_ = ResourceSource<RootSignature>::DefaultSampler(), .item_ { "b0VS,b1AL,b0PS" } });

		//		ResourceObjectManager<PipelineState> *const pipelineManager = ResourceObjectManager<PipelineState>::GetInstance();
		//
		//		ResourceSource<PipelineState> noneSource;
		//
		//		// ルートシグネチャの代入
		//		noneSource.rootSignature_ = rootSignature_;
		//
		//#pragma region InputLayout(インプットレイアウト)
		//
		//		// InputLayout
		//		const std::array<D3D12_INPUT_ELEMENT_DESC, 3u> inputElementDescs{
		//			D3D12_INPUT_ELEMENT_DESC{
		//				.SemanticName = "POSITION",
		//					.SemanticIndex = 0,
		//					.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		//					.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		//			},
		//			D3D12_INPUT_ELEMENT_DESC{
		//				.SemanticName = "NORMAL",
		//				.SemanticIndex = 0,
		//				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
		//				.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		//			},
		//			D3D12_INPUT_ELEMENT_DESC{
		//				.SemanticName = "TEXCOORD",
		//				.SemanticIndex = 0,
		//				.Format = DXGI_FORMAT_R32G32_FLOAT,
		//				.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		//			},
		//		};
		//		noneSource.inputElementDescs_.assign(inputElementDescs.begin(), inputElementDescs.end());
		//
		//#pragma endregion
		//
		//		pipeline_[0] = pipelineManager->Load(noneSource);

	}

	void SolEngine::ModelRender::clear()
	{
		for (auto &map : modelsBuffer_) {
			map.clear();
		}

	}

	void SolEngine::ModelRender::AddData(ModelManager::Handle model, const SoLib::Transform &transform)
	{
		for (const MeshManager::Handle mesh : model->meshHandleList_) {
			const MaterialManager::Handle material = mesh->materialhandle_;
			auto &buff = modelsBuffer_[static_cast<uint32_t>(material->blendMode_)];
			buff[&transform].push_back(mesh);
		}

	}

	void SolEngine::ModelRender::Sort()
	{
		for (auto &buffs : modelsBuffer_) {
			for (auto [transform, meshs] : buffs) {
				std::sort(meshs.begin(), meshs.end());
			}
		}
	}

	void SolEngine::ModelRender::Draw(const Camera3D &camera) const
	{
		assert(Model::GetPipelineType() == Model::PipelineType::kModel && "設定されたシグネチャがkModelではありません");

		commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());

		{
			for (auto [transform, meshs] : modelsBuffer_[static_cast<uint32_t>(Model::BlendMode::kNone)]) {

				commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform->GetGPUVirtualAddress());
				for (auto &mesh : meshs) {
					mesh->Draw(commandList_);
				}

			}
		}

	}
}