#include "ModelData.h"
#include "Material.h"
#include "Mesh.h"

namespace SolEngine {
	std::unique_ptr<ModelData> ResourceCreater<ModelData>::CreateObject(const ResourceSource<ModelData> &source) const {

		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();

		std::unique_ptr<ModelData> modelResult = std::make_unique<ModelData>();

		modelResult->rootNode_ = ModelNode::Create(scene->mRootNode);
		modelResult->skinCluster_ = ResourceObjectManager<SkinClusterBase>::GetInstance()->Load({ source.assimpHandle_ });

		// マテリアルマネージャのインスタンス
		SolEngine::ResourceObjectManager<SolEngine::Material> *const materialManager = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance();

		// マテリアルをロード
		for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
			materialManager->Load({ source.assimpHandle_, i });
		}

		// モデルデータマネージャのインスタンス
		SolEngine::ResourceObjectManager<SolEngine::Mesh> *const meshManager = SolEngine::ResourceObjectManager<SolEngine::Mesh>::GetInstance();

		// メモリ確保
		modelResult->meshHandleList_.reserve(scene->mNumMeshes);
		// モデルデータのロードと保存
		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			modelResult->meshHandleList_.push_back(meshManager->Load({ source.assimpHandle_, i }));
		}

		return std::move(modelResult);
	}

	void ModelData::Draw(const Transform &transform, const Camera3D &camera) const
	{
		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
		assert(Model::GetPipelineType() == Model::PipelineType::kModel && "設定されたシグネチャがkModelではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());
		for (auto &mesh : meshHandleList_) {
			commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, ::ModelNode::kIdentity_->GetGPUVirtualAddress());

			mesh->Draw(commandList);
		}
	}

	void ModelData::Draw(const SkinCluster &skinCluster, const Transform &transform, const Camera3D &camera) const
	{

		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

		assert(Model::GetPipelineType() == Model::PipelineType::kSkinModel && "設定されたシグネチャがkSkinModelではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kMatrixPalette, skinCluster.GetPalette().GetHeapRange().GetHandle(0).gpuHandle_);
		for (auto &mesh : meshHandleList_) {
			// commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
			//commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kSkinModel)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
			mesh->Draw(commandList, 1, &skinCluster.GetInfluence().GetVBView());
		}
	}

	void ModelData::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const
	{
		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

		assert((Model::GetPipelineType() == Model::PipelineType::kParticle or Model::GetPipelineType() == Model::PipelineType::kShadowParticle) && "設定されたシグネチャがkParticleではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);

		for (const auto &mesh : meshHandleList_) {
			commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, ModelNode::kIdentity_->GetGPUVirtualAddress());

			mesh->Draw(commandList, drawCount);
		}
	}


	void ModelData::Draw(const SkinCluster &skinCluster, const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const
	{
		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
		assert(Model::GetPipelineType() == Model::PipelineType::kSkinParticle && "設定されたシグネチャがkSkinParticleではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);
		commandList->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kMatrixPalette, skinCluster.GetPalette().GetHeapRange().GetHandle(0).gpuHandle_);
		for (auto &mesh : meshHandleList_) {
			mesh->Draw(commandList, drawCount, &skinCluster.GetInfluence().GetVBView());
		}
	}
}