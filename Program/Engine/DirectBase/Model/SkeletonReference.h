/// @file SkeletonReference.h
/// @brief スケルトンの情報源
/// @author ONUKI seiya
#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

#include "AssimpData.h"
#include "Mesh.h"

#include <d3d12.h>
#include "../Base/CBuffer.h"
#include "../Render/Camera.h"
#include "SkinClusterBase.h"
#include "SkeletonAnimation/ModelNode.h"

namespace SolEngine {

	class SkeletonJointReference : public IResourceObject {
		using MeshManager = ResourceObjectManager<Mesh>;

	public:

		struct ModelJointReference {

			// 名前
			std::string name_;
			// 子jointへのIndexリスト
			std::vector<uint32_t> children_;
			// 自分自身のIndex
			uint32_t index_;
			// 自身の親のIndex。存在しない場合はmax
			uint32_t parent_ = (std::numeric_limits<uint32_t>::max)();

		};

		// RootJointのIndex
		uint32_t root_ = 0u;
		// Joint名からIndexを返す辞書
		std::unordered_map<std::string, uint32_t> jointMap_;

		// 各ジョイントに対しての情報を持つ｡
		std::vector<std::unique_ptr<ModelJointReference>> joints_;

		/// @brief ジョイントを検索してそれを返す
		/// @param[in] jointName 検索するジョイントの名前
		/// @return 検索したジョイント
		ModelJointReference *GetJointData(const char *jointName) const;

		/// @brief 再起的にスケルトンを構築
		/// @param[in] node 元のノード
		/// @param[out] joints 出力先のジョイントリスト
		/// @param[in] parent 親のIndex
		/// @return 自分自身のIndex
		static uint32_t MakeJointIndex(const aiNode *node, std::vector<std::unique_ptr<ModelJointReference>> &joints, const uint32_t parent = (std::numeric_limits<uint32_t>::max)());


	};

	template <>
	class ResourceSource<SkeletonJointReference> {
	public:
		ResourceSource<SkeletonJointReference>() = default;
		ResourceSource<SkeletonJointReference>(const ResourceSource<SkeletonJointReference> &) = default;
		ResourceSource<SkeletonJointReference> &operator=(const ResourceSource<SkeletonJointReference> &) = default;
		ResourceSource<SkeletonJointReference>(const ResourceHandle<AssimpData> assimpHandle) : assimpHandle_(assimpHandle) {}

		// モデルのノード
		ResourceHandle<AssimpData> assimpHandle_;

		bool operator==(const ResourceSource<SkeletonJointReference> &) const = default;
	};

	template <>
	class ResourceCreater<SkeletonJointReference> {
	public:

		/// @brief リソースの生成
		/// @param[in] source リソースソース
		/// @return 生成したリソース
		std::unique_ptr<SkeletonJointReference> CreateObject(const ResourceSource<SkeletonJointReference> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::SkeletonJointReference>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::SkeletonJointReference> &data) const {
			return data.assimpHandle_.GetHashID();
		}
	};
}