/// @file ModelNode.h
/// @brief Skeletonアニメーション用ノード
/// @author ONUKI seiya
#pragma once
#include <memory>
#include <assimp/scene.h>
#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../../Base/CBuffer.h"
#include "../../Base/StructBuffer.h"
#include "../../Engine/ResourceObject/ResourceObject.h"
#include "../../Engine/ResourceObject/ResourceObjectManager.h"
#include "../AssimpData.h"

namespace SolEngine {

	namespace SkeletonAnimation {
		/// @brief ノードツリーの要素
		/// @details 親子関係を持つノードの単体
		class NodeTreeElement {
		public:
			NodeTreeElement() = default;
			NodeTreeElement(const NodeTreeElement &) = default;
			NodeTreeElement(NodeTreeElement &&) = default;
			~NodeTreeElement() = default;

			// 名前
			std::string name_{};
			// 子jointへのIndexリスト
			std::vector<uint32_t> children_{};
			// 自分自身のIndex
			uint32_t index_{};
			// 自身の親のIndex。存在しない場合はmax
			uint32_t parent_ = (std::numeric_limits<uint32_t>::max)();
		};

		/// @brief ノードツリー
		/// @details 親子関係を持つノードのツリー
		class NodeTree : public IResourceObject {
		public:

			// RootノードのIndex
			uint32_t root_ = 0u;
			// ノード名からIndexを返す辞書
			std::unordered_map<std::string, uint32_t> nodeMap_;
			/// @brief ノードを検索してそれを返す
			/// @param[in] nodeName 検索するノード名
			/// @param[in] data 検索するデータの参照元
			/// @return 検索結果
			template<SoLib::IsContainer T>
			auto *GetNodeData(const std::string &nodeName, T &data) const {
				// ノードを検索する
				auto itr = nodeMap_.find(nodeName);
				// ノードが存在したら返す
				return itr != nodeMap_.end() ? data[itr->second].get() : nullptr;
			}

			// 各ノードに対しての情報を持つ｡
			std::vector<std::unique_ptr<NodeTreeElement>> nodes_;

			// ノードを検索してそれを返す
			NodeTreeElement *GetNodeData(const std::string &nodeName) const;

			/// @brief 再起的にノードのIndexを作る
			/// @param[in] node	assimpのノード 
			/// @param[out] joints 書き込み先の配列
			/// @param[in] parent 親のIndex
			/// @return そのノードのIndex
			static uint32_t MakeNodeIndex(const aiNode *node, std::vector<std::unique_ptr<NodeTreeElement>> &joints, const uint32_t parent = (std::numeric_limits<uint32_t>::max)());

		private:
			/// @brief 再起的に姿勢データを作る
			/// @param[in] node assimpのノード
			/// @param[out] transVector 書き込み先の配列
			static void MakeTransData(const aiNode *node, std::vector<SoLib::SimpleTransformQuaternion> &transVector);

		};

	}

	using NodeTreeElement = SkeletonAnimation::NodeTreeElement;
	using NodeTree = SkeletonAnimation::NodeTree;

	template <>
	class ResourceSource<NodeTree> {
	public:
		ResourceSource<NodeTree>() = default;
		ResourceSource<NodeTree>(const ResourceSource<NodeTree> &) = default;
		ResourceSource<NodeTree> &operator=(const ResourceSource<NodeTree> &) = default;
		ResourceSource<NodeTree>(const ResourceHandle<AssimpData> assimpHandle) : assimpHandle_(assimpHandle) {}
		/// @brief Assimpのハンドル
		ResourceHandle<AssimpData> assimpHandle_;

		bool operator==(const ResourceSource<NodeTree> &) const = default;
	};

	template <>
	class ResourceCreater<NodeTree> {
	public:
		/// @brief リソースを作成する
		/// @param[in] source リソースソース
		/// @return 作成したリソース
		std::unique_ptr<NodeTree> CreateObject(const ResourceSource<NodeTree> &source) const;

	};

}

namespace std {
	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::NodeTree>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::NodeTree> &data) const {
			return data.assimpHandle_.GetHashID();
		}
	};
}