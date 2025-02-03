/// @file ModelNode.cpp
/// @brief Skeletonアニメーション用ノードの実装
/// @author ONUKI seiya
#include "ModelNode.h"
#include "../../Engine/Utils/Convert/Convert.h"

namespace SolEngine {
	namespace SkeletonAnimation {

		uint32_t NodeTree::MakeNodeIndex(const aiNode *node, std::vector<std::unique_ptr<NodeTreeElement>> &joints, const uint32_t parent)
		{
			// 領域の確保回数を減らす
			joints.reserve(joints.size() + 1 + node->mNumChildren);
			// ジョイントの構築
			auto joint = std::make_unique<NodeTreeElement>();
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
				uint32_t childIndex = NodeTree::MakeNodeIndex(child, joints, selfIndex);
				// 自分のデータに子のデータを書き込む
				joints[selfIndex]->children_.push_back(childIndex);
			}
			// 自分の番号を返す
			return selfIndex;
		}

		NodeTreeElement *NodeTree::GetNodeData(const std::string &nodeName) const
		{
			return GetNodeData(nodeName, nodes_);
		}

		void NodeTree::MakeTransData(const aiNode *node, std::vector<SoLib::SimpleTransformQuaternion> &transVector)
		{
			// 姿勢データを転送
			transVector.emplace_back() = SoLib::Convert<SoLib::SimpleTransformQuaternion>(node->mTransformation);
			const std::span<aiNode *> children = std::span<aiNode *>{ node->mChildren, node->mNumChildren };
			// 子のデータを書き込む
			for (const auto &child : children) {
				// 子のデータを追加
				NodeTree::MakeTransData(child, transVector);
			}
		}
	}

	std::unique_ptr<NodeTree> ResourceCreater<NodeTree>::CreateObject(const ResourceSource<NodeTree> &source) const {

		std::unique_ptr<NodeTree> result = std::make_unique<NodeTree>();

		// ノードからジョイントを構築し、現在のジョイントのindexを保存する
		result->root_ = SkeletonAnimation::NodeTree::MakeNodeIndex(source.assimpHandle_->importer_->GetScene()->mRootNode, result->nodes_);

		// 領域の確保
		result->nodeMap_.reserve(result->nodes_.size());

		// 名前とindexを紐づける
		std::transform(result->nodes_.cbegin(), result->nodes_.cend(), std::inserter(result->nodeMap_, result->nodeMap_.end()), [](auto &item) {return std::make_pair(item->name_, item->index_); });

		return result;

	}

}