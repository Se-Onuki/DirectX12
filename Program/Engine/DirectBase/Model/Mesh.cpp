#include "Mesh.h"

namespace SolEngine {
	std::unique_ptr<Mesh> ResourceCreater<Mesh>::CreateObject(const ResourceSource<Mesh> &source) const {

		static constexpr uint32_t kDefaultMaterialIndex = 0;

		const aiScene *const scene = source.assimpHandle->importer_->GetScene();

		const std::span<aiMesh *> sceneArr = { scene->mMeshes, scene->mNumMeshes };

		//for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		std::unique_ptr<Mesh> meshResult = std::make_unique<Mesh>();

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

		// マテリアルのポインタを取得
		//meshResult->material_ = result->materialMap_.at(SoLib::to_string(mesh->mMaterialIndex)).get();

		// SkinCluster構築用のデータ取得を追加
		//for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
		//	aiBone *aiBone = mesh->mBones[boneIndex];
		//	std::string jointName = aiBone->mName.C_Str();
		//	// 名前をもとにデータを構築
		//	JointWeightData &jointWeightData = result->skinCluster_.skinClusterData_[jointName];

		//	// バインド行列の逆行列を取得
		//	aiMatrix4x4 bindPoseMatrixAssimp = aiBone->mOffsetMatrix.Inverse();
		//	aiVector3D scale, translate;
		//	aiQuaternion rotate;
		//	// Transformを取得
		//	bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
		//	Matrix4x4 bindPoseMatrix = SoLib::Convert(scale, rotate, translate).Affine();
		//	// 逆行列を保存
		//	jointWeightData.inverseBindPoseMatrix_ = bindPoseMatrix.InverseSRT();

		//	// 領域を確保しておく
		//	jointWeightData.vertexWeightData_.resize(aiBone->mNumWeights);
		//	for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
		//		aiVertexWeight aiWeight = aiBone->mWeights[weightIndex];
		//		// ウェイトデータを取得して格納
		//		jointWeightData.vertexWeightData_[weightIndex] = { .weight_ = aiWeight.mWeight, .vertexIndex_ = aiWeight.mVertexId };
		//	}


		//}

		// 保存されたデータからメッシュを作成
		//meshResult->CreateBuffer();

		// 名前の設定
		//meshResult->meshName_ = mesh->mName.C_Str();

		// メッシュをコンテナに保存
		//result->meshList_.push_back(std::move(meshResult));
		//}

		return std::move(meshResult);
	}
}