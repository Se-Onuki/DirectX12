#include "SkeletonReference.h"

namespace SolEngine {

	ModelJointReference *SkeletonReference::GetJointData(const char *jointName) const
	{
		// データを検索する
		auto itr = jointMap_.find(jointName);

		// もし検索先がなかったら終わる
		if (itr == jointMap_.end()) { return nullptr; }
		// 範囲外であったら終わる
		if (joints_.size() <= itr->second) { return nullptr; }
		// データを取得して返す
		return joints_.at(itr->second).get();
	}


	std::unique_ptr<SkeletonReference> ResourceCreater<SkeletonReference>::CreateObject(const ResourceSource<SkeletonReference> &source) const
	{
		// 生成するデータ
		std::unique_ptr<SkeletonReference> result = std::make_unique<SkeletonReference>();

		// ノードからジョイントを構築し、現在のジョイントのindexを保存する
		result->root_ = ModelJointReference::MakeJointIndex(source.modelNode_, {}, result->joints_);

		// 領域の確保
		result->jointMap_.reserve(result->joints_.size());

		// 名前とindexを紐づける
		std::transform(result->joints_.cbegin(), result->joints_.cend(), std::inserter(result->jointMap_, result->jointMap_.end()), [](auto &item) {return std::make_pair(item->name_, item->index_); });


		// データを成功として返す
		return std::move(result);

	}

}
