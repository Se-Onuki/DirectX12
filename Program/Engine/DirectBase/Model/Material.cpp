#include "Material.h"

namespace SolEngine {
	std::unique_ptr<Material> ResourceCreater<Material>::CreateObject(const ResourceSource<Material> &source) const {

		static constexpr uint32_t kDefaultMaterialIndex = 0;

		const aiScene *const scene = source.assimpHandle->importer_->GetScene();

		const std::span<aiMesh *> sceneArr = { scene->mMeshes, scene->mNumMeshes };
		std::unique_ptr<Material> meshResult = std::make_unique<Material>();

		// メッシュのポインタ
		const aiMesh *const mesh = sceneArr[source.index_];
		assert(mesh->HasNormals() and "法線が無いメッシュは今回は非対応");
		assert(mesh->HasTextureCoords(kDefaultMaterialIndex) and "Texcoordの無いMeshは今回は非対応");

		meshResult->vertexBuffer_.Resize(mesh->mNumVertices);

		// 頂点として解析する
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
			// 3次元座標
			const aiVector3D &position = mesh->mVertices[vertexIndex];
			// 法線
			const aiVector3D &normal = mesh->mNormals[vertexIndex];
			// テクスチャ座標
			const aiVector3D &texcoord = mesh->mTextureCoords[kDefaultMaterialIndex][vertexIndex];

			// 頂点データ
			Mesh::VertexData &vertex = meshResult->vertexBuffer_.GetVertexData()[vertexIndex];
			vertex.position = { position.x, position.y, position.z, 1.f };
			vertex.normal = { normal.x, normal.y, normal.z };
			vertex.texCoord = { texcoord.x, texcoord.y };

			// データの補正
			// aiProcess_MakeLeftHandedは z *= -1 で、右手->左手に変換するので手動で対処
			vertex.position.x *= -1.f;
			vertex.normal.x *= -1.f;
		}
		// Faceの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			// Faceの参照
			const aiFace &face = mesh->mFaces[faceIndex];

			assert(face.mNumIndices == 3u and "三角形のみサポート");

			std::span<uint32_t> elements{ face.mIndices, face.mNumIndices };
			meshResult->indexBuffer_.SetIndexData(elements);
		}

		return std::move(meshResult);
	}
}