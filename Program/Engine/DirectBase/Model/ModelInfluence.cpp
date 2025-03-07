/// @file ModelInfluence.cpp
/// @brief モデルの影響情報
/// @author ONUKI seiya
#include "ModelInfluence.h"
#include "../Engine/DirectBase/Base/DirectXCommon.h"

namespace SolEngine {

	std::unique_ptr<ModelInfluence> ResourceCreater<ModelInfluence>::CreateObject(const ResourceSource<ModelInfluence> &source) const {

		std::unique_ptr<ModelInfluence> result = std::make_unique<ModelInfluence>();

		//// メッシュのソースデータ
		//const auto &meshSource = source.meshHandle_.GetSource();

		auto *const dxCommon = ::DirectXCommon::GetInstance();
		// SRVを取得する
		result->heapRange_ = dxCommon->GetSRVHeap()->RequestHeapAllocation(1u);

		auto *const vertexManager = ResourceObjectManager<ModelVertexData>::GetInstance();

		const auto &modelVertex = vertexManager->Load({ source.assimpHandle_ });
		// 頂点数と同じ長さのデータを構築する
		result->influence_.Resize(modelVertex->vertexBuffer_.GetVertexData().size());

		// デスクリプタヒープに保存
		dxCommon->GetDevice()->CreateShaderResourceView(result->influence_.GetVertexData().GetResources(), &result->influence_.GetVertexData().GetDesc(), result->heapRange_.GetHandle(0u).cpuHandle_);

		// 頂点の数を保存
		result->vertexCount_ = result->influence_.GetVertexData().size();

		ResourceObjectManager<SkeletonJointReference> *skeletonReferenceManager = ResourceObjectManager<SkeletonJointReference>::GetInstance();
		ResourceObjectManager<SkinClusterBase> *skinClusterManager = ResourceObjectManager<SkinClusterBase>::GetInstance();
		// スケルトンのソースデータ
		auto skeletonRef = skeletonReferenceManager->Load({ source.assimpHandle_ });
		// モデルデータのジョイント情報
		auto skinCluster = skinClusterManager->Load({ source.assimpHandle_ });

		// 番兵を取る
		const auto jointEndIt = skeletonRef->jointMap_.end();

		for (uint32_t i = 0; i < skinCluster->skinClusterData_.size(); i++) {
			const auto &cluster = skinCluster->skinClusterData_[i];
			const auto &vertexOffset = modelVertex->vertexOffsets_[i];

			// 書き込む先
			std::span<VertexInfluence> influence = { &result->influence_[vertexOffset.vertexOffset_], vertexOffset.vertexCount_ };

			// モデルデータを解析してInfluenceを埋める
			for (const auto &[keyName,  // ジョイント名
				jointWeight	// 各頂点のジョイントに対する重さ
			] : cluster) {
				// 一致するジョイントの対象が存在するか探す
				auto it = skeletonRef->jointMap_.find(keyName);
				if (it == jointEndIt) { // 存在しなかったら飛ばす
					continue;
				}

				for (const auto &vertexWeight : jointWeight.vertexWeightData_) {
					// 該当するinfluence情報を参照しておく
					auto &currentInfluence = influence[vertexWeight.vertexIndex_].vertexInfluence_;
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

		return std::move(result);

	}

}
