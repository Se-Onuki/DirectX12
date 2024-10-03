#include "ModelData.h"
#include "Material.h"
#include "Mesh.h"
#include <execution>
#include "SkeletonAnimation/Skeleton.h"
#include "../../Engine/Utils/Convert/Convert.h"

namespace SolEngine {
	std::unique_ptr<ModelData> ResourceCreater<ModelData>::CreateObject(const ResourceSource<ModelData> &source) const {

		// もしハンドルが空であったら終わる
		if (not source.assimpHandle_) { return nullptr; }

		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();
		// もしデータが読めなかったら終わる
		if (not scene) { return nullptr; }

		// 構築したデータ
		std::unique_ptr<ModelData> result = std::make_unique<ModelData>();
		// 頂点データマネージャのインスタンス
		SolEngine::ResourceObjectManager<SolEngine::ModelVertexData> *const vertexManager = SolEngine::ResourceObjectManager<SolEngine::ModelVertexData>::GetInstance();
		// モデルに紐づいた頂点データの構築
		result->modelVertex_ = vertexManager->Load({ source.assimpHandle_ });

		// マテリアルマネージャのインスタンス
		SolEngine::ResourceObjectManager<SolEngine::Material> *const materialManager = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance();

		// マテリアルをロード
		for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
			materialManager->Load({ source.assimpHandle_, i });
		}

		// モデルデータマネージャのインスタンス
		SolEngine::ResourceObjectManager<SolEngine::Mesh> *const meshManager = SolEngine::ResourceObjectManager<SolEngine::Mesh>::GetInstance();

		// メモリ確保
		result->meshHandleList_.reserve(scene->mNumMeshes);
		// モデルデータのロードと保存
		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			result->meshHandleList_.push_back(meshManager->Load({ source.assimpHandle_, i }));
		}
		result->rootNode_ = SoLib::Convert<Matrix4x4>(scene->mRootNode->mTransformation);

		return std::move(result);
	}

	void ModelData::Draw(const Transform &transform, const Camera3D &camera) const
	{
		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
		assert(Model::GetPipelineType() == Model::PipelineType::kModel && "設定されたシグネチャがkModelではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kWorldTransform, transform.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, rootNode_.GetGPUVirtualAddress());
		for (auto &mesh : meshHandleList_) {

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
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, rootNode_.GetGPUVirtualAddress());
		for (auto &mesh : meshHandleList_) {

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

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, rootNode_.GetGPUVirtualAddress());
		for (const auto &mesh : meshHandleList_) {

			mesh->Draw(commandList, 1, &skinCluster.skeletonRef_->modelInfluence_->influence_.GetVBView());
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
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, rootNode_.GetGPUVirtualAddress());

		for (const auto &mesh : meshHandleList_) {

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
		commandList->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kMatrixPalette, skinCluster.GetPalette().GetHeapRange().GetHandle().gpuHandle_);
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, rootNode_.GetGPUVirtualAddress());
		for (const auto &mesh : meshHandleList_) {
			mesh->Draw(commandList, drawCount, &skinCluster.skeletonRef_->modelInfluence_->influence_.GetVBView());
		}
	}
}