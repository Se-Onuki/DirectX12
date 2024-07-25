#include "MeshInfluence.h"

namespace SolEngine {

	std::unique_ptr<MeshInfluence> ResourceCreater<MeshInfluence>::CreateObject(const ResourceSource<MeshInfluence> &source) const {

		// 生成するデータ
		std::unique_ptr<MeshInfluence> result = std::make_unique<MeshInfluence>();

		// メッシュのソースデータ
		const auto &meshSource = source.meshHandle_.GetSource();
		// 頂点数と同じ長さのデータを構築する
		//result->influence_.Resize(source.meshHandle_->vertexBuffer_.GetVertexData().size());

		// 番兵を取る
		const auto jointEndIt = source.skeletonReference_->jointMap_.end();

		// スキンクラスターから､該当するメッシュの情報を取得する
		const auto &clusterItr = source.skinClusterBase_->skinClusterData_.at(meshSource->index_);
		// メッシュにデータが保存されてなかったら飛ばす
		//if (not clusterItr) { return nullptr; }

		// モデルデータを解析してInfluenceを埋める
		for (const auto &[keyName,  // ジョイント名
			jointWeight	// 各頂点のジョイントに対する重さ
		] : clusterItr) {
			// 一致するジョイントの対象が存在するか探す
			auto it = source.skeletonReference_->jointMap_.find(keyName);
			if (it == jointEndIt) { // 存在しなかったら飛ばす
				continue;
			}

			for (const auto &vertexWeight : jointWeight.vertexWeightData_) {
				// 該当するinfluence情報を参照しておく
				auto &currentInfluence = result->influence_[vertexWeight.vertexIndex_].vertexInfluence_;
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

		//}




		// 生成したデータを返す
		return std::move(result);

	}

}