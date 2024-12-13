/// @file SkeletonReference.cpp
/// @brief スケルトンの情報源
/// @author ONUKI seiya
#include "SkeletonReference.h"

namespace SolEngine {

	SkeletonJointReference::ModelJointReference *SkeletonJointReference::GetJointData(const char *jointName) const
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


	std::unique_ptr<SkeletonJointReference> ResourceCreater<SkeletonJointReference>::CreateObject(const ResourceSource<SkeletonJointReference> &source) const
	{
		// 生成するデータ
		std::unique_ptr<SkeletonJointReference> result = std::make_unique<SkeletonJointReference>();

		// ノードからジョイントを構築し、現在のジョイントのindexを保存する
		result->root_ = SkeletonJointReference::MakeJointIndex(source.assimpHandle_->importer_->GetScene()->mRootNode, result->joints_);

		// 領域の確保
		result->jointMap_.reserve(result->joints_.size());

		// 名前とindexを紐づける
		std::transform(result->joints_.cbegin(), result->joints_.cend(), std::inserter(result->jointMap_, result->jointMap_.end()), [](auto &item) {return std::make_pair(item->name_, item->index_); });


		// データを成功として返す
		return std::move(result);

	}

	uint32_t SkeletonJointReference::MakeJointIndex(const aiNode *node, std::vector<std::unique_ptr<ModelJointReference>> &joints, const uint32_t parent)
	{
		// 領域の確保回数を減らす
		joints.reserve(joints.size() + 1 + node->mNumChildren);
		// ジョイントの構築
		auto joint = std::make_unique<ModelJointReference>();
		// 名前の転送
		joint->name_ = node->mName.C_Str();
		// 現在のジョイント数を計算
		const uint32_t selfIndex = joint->index_ = static_cast<uint32_t>(joints.size());
		// 親が存在するなら親のデータを代入
		if (parent != (std::numeric_limits<uint32_t>::max)()) { joint->parent_ = parent; }

		// 書き込んだデータを追加
		joints.push_back(std::move(joint));
		// 子の配列
		const std::span<aiNode *> children = std::span<aiNode *>{ node->mChildren, node->mNumChildren };
		// 子のデータを書き込む
		for (const auto &child : children) {
			// 子のIndexを返して､子のデータを構築
			uint32_t childIndex = MakeJointIndex(child, joints, selfIndex);
			// 自分のデータに子のデータを書き込む
			joints[selfIndex]->children_.push_back(childIndex);
		}
		// 自分の番号を返す
		return selfIndex;
	}

}
