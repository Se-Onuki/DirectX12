#include "SkinClusterBase.h"
#include "../../../Utils/Convert/Convert.h"
#include "Model.h"

namespace SolEngine {
	std::unique_ptr<SkinClusterBase> ResourceCreater<SkinClusterBase>::CreateObject(const ResourceSource<SkinClusterBase> &source) const {

		std::unique_ptr<SkinClusterBase> result = std::make_unique<SkinClusterBase>();

		const auto *const scene = source.assimpHandle_->importer_->GetScene();
		// シーンの読み込みに成功したら
		if (scene) {

			result->skinCluster_ = std::make_unique<SkinClusterBaseData>();

			std::span<aiMesh *> meshList = { scene->mMeshes, scene->mNumMeshes };

			for (const aiMesh *const mesh : meshList) {

				// SkinCluster構築用のデータ取得を追加
				for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
					aiBone *aiBone = mesh->mBones[boneIndex];
					std::string jointName = aiBone->mName.C_Str();
					// 名前をもとにデータを構築
					JointWeightData &jointWeightData = result->skinCluster_->skinClusterData_[jointName];

					// バインド行列の逆行列を取得
					aiMatrix4x4 bindPoseMatrixAssimp = aiBone->mOffsetMatrix.Inverse();
					aiVector3D scale, translate;
					aiQuaternion rotate;
					// Transformを取得
					bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
					Matrix4x4 bindPoseMatrix = SoLib::Convert(scale, rotate, translate).Affine();
					// 逆行列を保存
					jointWeightData.inverseBindPoseMatrix_ = bindPoseMatrix.InverseSRT();

					// 領域を確保しておく
					jointWeightData.vertexWeightData_.resize(aiBone->mNumWeights);
					for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
						aiVertexWeight aiWeight = aiBone->mWeights[weightIndex];
						// ウェイトデータを取得して格納
						jointWeightData.vertexWeightData_[weightIndex] = { .weight_ = aiWeight.mWeight, .vertexIndex_ = aiWeight.mVertexId };
					}

				}
			}
		}

		return std::move(result);

	}

}