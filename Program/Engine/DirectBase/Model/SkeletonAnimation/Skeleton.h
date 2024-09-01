#pragma once
#include <memory>
#include "../SkinClusterBase.h"
#include "ModelNode.h"
#include "../SkeletonReference.h"
#include "../ModelInfluence.h"
#include "../AssimpData.h"

namespace SolEngine {

	namespace SkeletonAnimation {

		class Skeleton : IResourceObject {
		public:
			ResourceHandle<SkinClusterBase> skinCluster_;				// 姿勢データ
			ResourceHandle<SkeletonJointReference> skeletonReference_;	// ジョイントの関係性
			ResourceHandle<ModelInfluence> modelInfluence_;				// 頂点の影響度


		};

	}

	using Skeleton = SkeletonAnimation::Skeleton;

	template <>
	class ResourceSource<Skeleton> {
	public:
		ResourceSource<Skeleton>() = default;
		ResourceSource<Skeleton>(const ResourceSource<Skeleton> &) = default;
		ResourceSource<Skeleton> &operator=(const ResourceSource<Skeleton> &) = default;
		ResourceSource<Skeleton>(const ResourceHandle<AssimpData> assimpHandle) : assimpHandle_(assimpHandle) {}

		// ファイルのハンドル
		ResourceHandle<AssimpData> assimpHandle_;

		bool operator==(const ResourceSource<Skeleton> &) const = default;
	};

	template <>
	class ResourceCreater<Skeleton> {
	public:

		std::unique_ptr<Skeleton> CreateObject(const ResourceSource<Skeleton> &source) const;

	};

}

namespace std {
	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Skeleton>> {

		size_t operator()(const SolEngine::ResourceSource<SolEngine::Skeleton> &data) const {
			return data.assimpHandle_.GetHashID();
		}

	};
}