#include "ModelVertexData.h"

namespace SolEngine {

	std::unique_ptr<ModelVertexData> ResourceCreater<ModelVertexData>::CreateObject(const ResourceSource<ModelVertexData> &source) const {

		// モデルの頂点リスト
		std::unique_ptr<ModelVertexData> result = std::make_unique<ModelVertexData>();

		const aiScene *const scene = source.assimpHandle_->importer_->GetScene();

		//const uint32_t materialCount = scene->mNumMaterials;

		const std::span<aiMesh *> meshes = { scene->mMeshes, scene->mNumMeshes };

		// 全ての頂点数
		std::pair<uint32_t,uint32_t> vertexCount{};
		// 頂点の数を計算する
		for (const aiMesh *const mesh : meshes) {
			// 追加するデータ
			const ModelVertexData::VertexOffset addCount{ vertexCount.first, vertexCount.second, mesh->mNumVertices, mesh->mNumFaces * 3u };

			// 開始する頂点数とindex数を保存する
			result->vertexOffsets_.push_back(addCount);

			// 頂点数
			vertexCount.first += addCount.vertexCount_;
			vertexCount.second += addCount.indexCount_;
		}

		// 頂点数分のメモリを確保
		result->vertexBuffer_.CreateBuffer(vertexCount.first);
		result->indexBuffer_.CreateBuffer(vertexCount.second);

		//std::vector<std::pair<std::span<ModelVertexData::VertexData>, std::span<uint32_t>>> outputTargets{ meshes.size() };

		//std::transform(result->vertexOffsets_.begin(), result->vertexOffsets_.end(), meshes.begin(), outputTargets.begin(), [&result](const auto &offset, const auto &mesh)->std::pair<std::span<ModelVertexData::VertexData>, std::span<uint32_t>> {

		//	// 書き込み先の頂点データ
		//	std::span<ModelVertexData::VertexData> targetVertex = { &result->vertexBuffer_[offset.vertex_], mesh->mNumVertices };
		//	std::span<uint32_t> targetIndex = { &result->indexBuffer_[offset.index_], mesh->mNumVertices };

		//	return std::make_pair(targetVertex, targetIndex);
		//	});

		//std::transform(meshes.begin(), meshes.end(), outputTargets.begin(), []() {});

		for (uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++) {

			// メッシュのデータ
			const aiMesh *const mesh = meshes[meshIndex];

			// 頂点データのオフセット
			const ModelVertexData::VertexOffset vertexOffset = result->vertexOffsets_[meshIndex];

			// 書き込み先の頂点データ
			std::span<ModelVertexData::VertexData> targetVertex = { &result->vertexBuffer_[vertexOffset.vertexOffset_], vertexOffset.vertexCount_ };
			std::span<uint32_t> targetIndex = { &result->indexBuffer_[vertexOffset.indexOffset_], vertexOffset.indexCount_ };

			// メッシュのポインタ
			assert(mesh->HasNormals() and "法線が無いメッシュは今回は非対応");

			// 頂点として解析する
			for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {

				// 3次元座標
				const aiVector3D &position = mesh->mVertices[vertexIndex];
				// 法線
				const aiVector3D &normal = mesh->mNormals[vertexIndex];

				// 頂点データ
				auto &vertex = targetVertex[vertexIndex];
				vertex.position = { position.x, position.y, position.z, 1.f };
				vertex.normal = { normal.x, normal.y, normal.z };
				// データの補正
				// aiProcess_MakeLeftHandedは z *= -1 で、右手->左手に変換するので手動で対処
				vertex.position.x *= -1.f;
				vertex.normal.x *= -1.f;

				// テクスチャ座標を持っているか
				if (mesh->HasTextureCoords(0)) {
					// テクスチャ座標
					const aiVector3D &texcoord = mesh->mTextureCoords[0][vertexIndex];

					vertex.texCoord = { texcoord.x, texcoord.y };
				}

			}

			// Faceの解析
			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
				// Faceの参照
				const aiFace &face = mesh->mFaces[faceIndex];

				assert(face.mNumIndices == 3u and "三角形のみサポート");

				const std::span<uint32_t> elements{ face.mIndices, face.mNumIndices };
				for (uint32_t i = 0u; i < 3u; i++) {
					targetIndex[faceIndex * 3u + i] = elements[i];
				}
			}

		}

		return std::move(result);

	}

}
