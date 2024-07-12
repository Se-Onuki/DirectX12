#include "ModelData.h"
#include "Material.h"
#include "Mesh.h"
#include <execution>

namespace SolEngine {
	std::unique_ptr<ModelData> ResourceCreater<ModelData>::CreateObject(const ResourceSource<ModelData> &source) const {

		// もしハンドルが空であったら終わる
		if (not source.assimpHandle_) { return nullptr; }

		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();
		// もしデータが読めなかったら終わる
		if (not scene) { return nullptr; }

		std::unique_ptr<ModelData> modelResult = std::make_unique<ModelData>();

		modelResult->rootNode_ = ModelNode::Create(scene->mRootNode);
		modelResult->skinCluster_ = ResourceObjectManager<SkinClusterBase>::GetInstance()->Load({ source.assimpHandle_ });
		modelResult->skeletonReference_ = ResourceObjectManager<SkeletonReference>::GetInstance()->Load(ResourceSource<SkeletonReference>{ modelResult->rootNode_.get() });
		SolEngine::ResourceObjectManager<SolEngine::ModelVertexData> *const vertexManager = SolEngine::ResourceObjectManager<SolEngine::ModelVertexData>::GetInstance();

		modelResult->modelVertex_ = vertexManager->Load({ source.assimpHandle_ });

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

		auto *const modelInfluenceManager = ResourceObjectManager<ModelInfluence>::GetInstance();

		const auto modelInfluence = modelInfluenceManager->Load({ source.assimpHandle_,modelResult->skinCluster_, modelResult->skeletonReference_ });

		modelResult->modelInfluence_ = modelInfluence;

		//// メッシュ影響度のマネージャ
		//ResourceObjectManager<MeshInfluence> *const meshInfluenceManager = ResourceObjectManager<MeshInfluence>::GetInstance();
		//// 影響度のソースを作成
		//std::vector<ResourceSource<MeshInfluence>> influenceSources;
		//// 領域を確保
		//influenceSources.resize(scene->mNumMeshes);
		//// データを保存する
		//std::transform(std::execution::par_unseq, modelResult->meshHandleList_.begin(), modelResult->meshHandleList_.end(), influenceSources.begin(), [&](const ResourceObjectManager<Mesh>::Handle mesh)->ResourceSource<MeshInfluence> {
		//	return ResourceSource<MeshInfluence>{ mesh, modelResult->skinCluster_, modelResult->skeletonReference_ };
		//	});
		//// データを構築し､保存する
		//modelResult->meshInfluenceList_ = meshInfluenceManager->Load(influenceSources);

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

	void ModelData::Draw(const VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> &vertexBuffer, const Transform &transform, const Camera3D &camera) const
	{
		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
		assert(Model::GetPipelineType() == Model::PipelineType::kModel && "設定されたシグネチャがkModelではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());
		for (auto &mesh : meshHandleList_) {
			commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, ::ModelNode::kIdentity_->GetGPUVirtualAddress());

			mesh->Draw(commandList, 1, vertexBuffer);
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

		for (uint32_t i = 0; i < meshHandleList_.size(); i++) {
			auto &mesh = meshHandleList_[i];
			// commandList_->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, mesh->pNode_->GetLocalMatrix().GetGPUVirtualAddress());
			//commandList_->SetPipelineState(graphicsPipelineState_[static_cast<uint32_t>(PipelineType::kSkinModel)][static_cast<uint32_t>(mesh->GetMaterial()->blendMode_)].Get()); // PSOを設定
			mesh->Draw(commandList, 1, &modelInfluence_->influence_.GetVBView());
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
			commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, this->rootNode_->GetLocalMatrix().GetGPUVirtualAddress());

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
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, ModelNode::kIdentity_->GetGPUVirtualAddress());
		for (uint32_t i = 0; i < meshHandleList_.size(); i++) {
			auto &mesh = meshHandleList_[i];
			mesh->Draw(commandList, drawCount, &modelInfluence_->influence_.GetVBView());
		}
	}
}