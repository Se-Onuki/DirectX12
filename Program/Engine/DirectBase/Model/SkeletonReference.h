#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

#include "AssimpData.h"
#include "Mesh.h"
#include "Model.h"

#include <d3d12.h>
#include "../Base/CBuffer.h"
#include "../Render/Camera.h"
#include "SkinClusterBase.h"

namespace SolEngine {

	class SkeletonReference : public IResourceObject {
		using MeshManager = ResourceObjectManager<Mesh>;

	public:

		// RootJointのIndex
		uint32_t root_ = 0u;
		// Joint名からIndexを返す辞書
		std::unordered_map<std::string, uint32_t> jointMap_;

		// 各ジョイントに対しての情報を持つ｡
		std::vector<std::unique_ptr<ModelJointReference>> joints_;

		// ジョイントを検索してそれを返す
		ModelJointReference *GetJointData(const char *jointName) const;


	};

	template <>
	class ResourceSource<SkeletonReference> {
	public:
		ResourceSource<SkeletonReference>() = default;
		ResourceSource<SkeletonReference>(const ResourceSource<SkeletonReference> &) = default;
		ResourceSource<SkeletonReference>(const ModelNode * data) :modelNode_(data) {}

		// モデルのノード
		const ModelNode* modelNode_;

		bool operator==(const ResourceSource<SkeletonReference> &) const = default;
	};

	template <>
	class ResourceCreater<SkeletonReference> {
	public:

		std::unique_ptr<SkeletonReference> CreateObject(const ResourceSource<SkeletonReference> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::SkeletonReference>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::SkeletonReference> &data) const {
			return size_t{ reinterpret_cast<size_t>(data.modelNode_) };
		}
	};
}