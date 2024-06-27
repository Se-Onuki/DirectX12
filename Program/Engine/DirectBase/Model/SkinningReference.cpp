#include "SkinningReference.h"

namespace SolEngine {

	std::unique_ptr<SkinningReference> ResourceCreater<SkinningReference>::CreateObject(const ResourceSource<SkinningReference> &source) const {

		// 生成したデータ
		std::unique_ptr<SkinningReference> result = std::make_unique<SkinningReference>();

		// スケルトンのベースデータの構築
		result->skeletonReference_ = ResourceObjectManager<SkeletonReference>::GetInstance()->Load(source.modelHandle_->rootNode_.get());

		// 各領域にデータを追加
		for (const auto &mesh : source.modelHandle_->meshHandleList_) {
			// データを末尾に構築
			auto &back = result->influenceList_.emplace_back();
			// メッシュを保存
			back.first = mesh;
			// 頂点データの領域を確保
			back.second.Resize(mesh->vertexBuffer_.GetVertexData().size());
		}
		/*std::transform(source.modelHandle_->meshHandleList_.begin(), source.modelHandle_->meshHandleList_.end(), std::back_inserter(result->influenceList_), [](auto mesh) {return std::make_pair(mesh, ); });*/

		// 番兵を取る
		const auto jointEndIt = result->skeletonReference_->jointMap_.end();

		// メッシュのIndexを回す
		for (uint32_t meshIndex = 0; meshIndex < source.modelHandle_->meshHandleList_.size(); meshIndex++) {

			// そのメッシュにデータが格納されているか
			const auto &clusterItr = source.modelHandle_->skinCluster_->skinClusterData_.at(meshIndex);
			// メッシュにデータが保存されてなかったら飛ばす
			if (not clusterItr) { continue; }

			// モデルデータを解析してInfluenceを埋める
			for (const auto &[keyName,  // ジョイント名
				jointWeight	// 各頂点のジョイントに対する重さ
			] : *clusterItr) {
				// 一致するジョイントの対象が存在するか探す
				auto it = result->skeletonReference_->jointMap_.find(keyName);
				if (it == jointEndIt) { // 存在しなかったら飛ばす
					continue;
				}

				for (const auto &vertexWeight : jointWeight.vertexWeightData_) {
					// 該当するinfluence情報を参照しておく
					auto &currentInfluence = result->influenceList_[meshIndex].second[vertexWeight.vertexIndex_].vertexInfluence_;
					// 挿入できる領域を検索する
					for (uint32_t index = 0; index < VertexInfluence::kNumMaxInfluence_; index++) {
						// 空いているところにデータを代入
						if (currentInfluence.weight_[index] == 0.0f) {
							currentInfluence.weight_[index] = vertexWeight.weight_;
							currentInfluence.vertexIndex_[index] = it->second;
							break;
						}

					}
				}
			}

		}


		// データを返す
		return std::move(result);

	}
}