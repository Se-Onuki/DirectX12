/// @file Mesh.cpp
/// @brief メッシュ
/// @author ONUKI seiya
#include "Mesh.h"
#include "../Base/TextureManager.h"

namespace SolEngine {
	void Mesh::Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount, const VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> &vertex) const
	{
		const auto &vertexOffset = modelVertex_->vertexOffsets_[meshIndex_];


		commandList->SetPipelineState(Model::GetGraphicsPipelineState()[static_cast<uint32_t>(Model::GetPipelineType())][static_cast<uint32_t>(materialhandle_->blendMode_)].Get()); // PSOを設定
		::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kTexture, materialhandle_->texHandle_);
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, materialhandle_->materialData_.GetGPUVirtualAddress());
		commandList->IASetVertexBuffers(0, 1, &vertex.GetVBView());
		commandList->IASetIndexBuffer(&modelVertex_->indexBuffer_.GetIBView());
		commandList->DrawIndexedInstanced(vertexOffset.indexCount_, drawCount, vertexOffset.indexOffset_, vertexOffset.vertexOffset_, 0);
	}
	void Mesh::Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount, const D3D12_VERTEX_BUFFER_VIEW *vbv) const {

		std::array<D3D12_VERTEX_BUFFER_VIEW, 2u> vbvs = {  };
		vbvs[0] = modelVertex_->vertexBuffer_.GetVBView();
		//vbvs[1] = texcoordBuffer_.GetVBView();
		if (vbv) {
			vbvs[1] = *vbv;
		}

		const auto &vertexOffset = modelVertex_->vertexOffsets_[meshIndex_];


		commandList->SetPipelineState(Model::GetGraphicsPipelineState()[static_cast<uint32_t>(Model::GetPipelineType())][static_cast<uint32_t>(materialhandle_->blendMode_)].Get()); // PSOを設定
		::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable((uint32_t)Model::RootParameter::kTexture, materialhandle_->texHandle_);
		commandList->SetGraphicsRootConstantBufferView((uint32_t)Model::RootParameter::kMaterial, materialhandle_->materialData_.GetGPUVirtualAddress());
		commandList->IASetVertexBuffers(0, vbv ? 2 : 1, vbvs.data());
		commandList->IASetIndexBuffer(&modelVertex_->indexBuffer_.GetIBView());
		commandList->DrawIndexedInstanced(vertexOffset.indexCount_, drawCount, vertexOffset.indexOffset_, vertexOffset.vertexOffset_, 0);
	}

	std::unique_ptr<Mesh> ResourceCreater<Mesh>::CreateObject(const ResourceSource<Mesh> &source) const {

		//static constexpr uint32_t kDefaultMaterialIndex = 0;

		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();

		//const uint32_t materialCount = scene->mNumMaterials;

		const std::span<aiMesh *> meshes = { scene->mMeshes, scene->mNumMeshes };

		std::unique_ptr<Mesh> meshResult = std::make_unique<Mesh>();

		// メッシュのポインタ
		const aiMesh *const mesh = meshes[source.index_];

		auto *const vertexManager = ResourceObjectManager<ModelVertexData>::GetInstance();

		const auto vertexData = vertexManager->Load({ source.assimpHandle_ });

		//const auto &vertexOffset = vertexData->vertexOffsets_[source.index_];

		// 頂点データを渡す
		meshResult->modelVertex_ = *vertexData;
		// メッシュ番号を渡す
		meshResult->meshIndex_ = source.index_;

		//assert(mesh->HasNormals() and "法線が無いメッシュは今回は非対応");

		//meshResult->vertexBuffer_.Resize(mesh->mNumVertices);
		////meshResult->texcoordBuffer_.Resize(mesh->mNumVertices);

		//// 頂点として解析する
		//for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {

		//	{
		//		// 3次元座標
		//		const aiVector3D &position = mesh->mVertices[vertexIndex];
		//		// 法線
		//		const aiVector3D &normal = mesh->mNormals[vertexIndex];

		//		// 頂点データ
		//		auto &vertex = meshResult->vertexBuffer_.GetVertexData()[vertexIndex];
		//		vertex.position = { position.x, position.y, position.z, 1.f };
		//		vertex.normal = { normal.x, normal.y, normal.z };
		//		// データの補正
		//		// aiProcess_MakeLeftHandedは z *= -1 で、右手->左手に変換するので手動で対処
		//		vertex.position.x *= -1.f;
		//		vertex.normal.x *= -1.f;



		//		//for (uint32_t materialIndex = 0u; materialIndex < materialCount and materialIndex < 4; materialIndex++) {
		//			//assert(mesh->HasTextureCoords(materialIndex) and "Texcoordの無いMeshは今回は非対応");

		//		if (mesh->HasTextureCoords(0)) {
		//			// テクスチャ座標
		//			const aiVector3D &texcoord = mesh->mTextureCoords[0][vertexIndex];

		//			vertex.texCoord = { texcoord.x, texcoord.y };
		//		}
		//		else {
		//			meshResult->hasTexcoord_ = false;
		//		}
		//	}
		//}

		//meshResult->indexBuffer_.Resize(mesh->mNumFaces * 3u);

		//// Faceの解析
		//for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
		//	// Faceの参照
		//	const aiFace &face = mesh->mFaces[faceIndex];

		//	assert(face.mNumIndices == 3u and "三角形のみサポート");

		//	const std::span<uint32_t> elements{ face.mIndices, face.mNumIndices };
		//	for (uint32_t i = 0u; i < 3u; i++) {
		//		meshResult->indexBuffer_.GetIndexData()[faceIndex * 3u + i] = elements[i];
		//	}
		//}

		// マテリアルのハンドルを取得
		meshResult->materialhandle_ = ResourceObjectManager<Material>::GetInstance()->Find({ .assimpHandle_ = source.assimpHandle_, .index_ = mesh->mMaterialIndex });

		return std::move(meshResult);
	}
}