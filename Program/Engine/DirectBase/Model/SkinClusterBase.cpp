#include "SkinClusterBase.h"
#include "../../../Utils/Convert/Convert.h"
#include "Model.h"
#include <execution>

namespace SolEngine {
	std::unique_ptr<SkinClusterBase> ResourceCreater<SkinClusterBase>::CreateObject(const ResourceSource<SkinClusterBase> &source) const {

		std::unique_ptr<SkinClusterBase> result = std::make_unique<SkinClusterBase>();

		const auto *const scene = source.assimpHandle_->importer_->GetScene();
		// シーンの読み込みに成功したら
		if (scene) {

			// メッシュの配列を生成する
			std::span<aiMesh *> meshList = { scene->mMeshes, scene->mNumMeshes };

			// メッシュの配列からボーンデータを抽出する｡
			for (const aiMesh *const mesh : meshList) {

				// メッシュに含まれたボーンデータの配列
				std::span<aiBone *> aiBones = { mesh->mBones, mesh->mNumBones };

				// SkinCluster構築用のデータ取得を追加
				for (const aiBone *const aiBone : aiBones) {
					// ボーンの名前を保存
					std::string jointName = aiBone->mName.C_Str();
					// 名前をもとにデータを構築
					JointWeightData &jointWeightData = result->skinClusterData_[jointName];

					// バインド行列の逆行列を取得
					const aiMatrix4x4 bindPoseMatrixAssimp = aiMatrix4x4{ aiBone->mOffsetMatrix }.Inverse();
					aiVector3D scale, translate;
					aiQuaternion rotate;
					// Transformを取得
					bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
					const Matrix4x4 bindPoseMatrix = SoLib::Convert(scale, rotate, translate).Affine();
					// 逆行列を保存
					jointWeightData.inverseBindPoseMatrix_ = bindPoseMatrix.InverseSRT();

					const size_t jointWeightSize = jointWeightData.vertexWeightData_.size();

					// 領域を確保しておく
					jointWeightData.vertexWeightData_.resize(jointWeightSize + aiBone->mNumWeights);

					// 頂点データの配列
					const std::span<aiVertexWeight> weights{ aiBone->mWeights, aiBone->mNumWeights };

					// 領域へのデータの格納
					std::transform(std::execution::par_unseq, weights.begin(), weights.end(), jointWeightData.vertexWeightData_.begin() + jointWeightSize, [](aiVertexWeight weight) {return VertexWeightData<>{.weight_ = weight.mWeight, .vertexIndex_ = weight.mVertexId }; });


				}
			}
		}

		return std::move(result);

	}

}