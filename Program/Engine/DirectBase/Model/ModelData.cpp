#include "ModelData.h"
#include "Material.h"
#include "Mesh.h"

namespace SolEngine {
	std::unique_ptr<ModelData> ResourceCreater<ModelData>::CreateObject(const ResourceSource<ModelData> &source) const {

		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();

		const std::span<aiMesh *> sceneArr = { scene->mMeshes, scene->mNumMeshes };

		std::unique_ptr<ModelData> meshResult = std::make_unique<ModelData>();

		SolEngine::ResourceObjectManager<SolEngine::Material> *const materialManager = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance();

		for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
			materialManager->Load({ source.assimpHandle_, i });
		}

		SolEngine::ResourceObjectManager<SolEngine::Mesh> *const meshManager = SolEngine::ResourceObjectManager<SolEngine::Mesh>::GetInstance();

		//meshResult->meshHandleList_.reserve(scene->mNumMeshes);

		meshResult->materialCount_ = scene->mNumMeshes;

		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			meshResult->meshHandleList_[i] = meshManager->Load({ source.assimpHandle_, i });
		}

		return std::move(meshResult);
	}
	void ModelData::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const
	{
		DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
		ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

		assert((Model::GetPipelineType() == Model::PipelineType::kParticle || Model::GetPipelineType() == Model::PipelineType::kShadowParticle) && "設定されたシグネチャがkParticleではありません");

		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kViewProjection, camera.constData_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kInstanceLocation, drawIndex.GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kWorldTransform, transformSRV);

		for (const auto &mesh : meshHandleList_) {
			commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kModelTransform, ModelNode::kIdentity_->GetGPUVirtualAddress());

			mesh->Draw(commandList, drawCount);
		}
	}
}