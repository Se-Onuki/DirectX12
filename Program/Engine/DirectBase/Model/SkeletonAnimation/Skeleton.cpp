/// @file Skeleton.cpp
/// @brief スケルトンの実装
/// @author ONUKI seiya
#include "Skeleton.h"

namespace SolEngine {

	std::unique_ptr<Skeleton> ResourceCreater<Skeleton>::CreateObject(const ResourceSource<Skeleton> &source) const {

		std::unique_ptr<Skeleton> result = std::make_unique<Skeleton>();

		ResourceObjectManager<ModelInfluence> *modelInfluenceManager = ResourceObjectManager<ModelInfluence>::GetInstance();
		ResourceObjectManager<SkeletonJointReference> *skeletonReferenceManager = ResourceObjectManager<SkeletonJointReference>::GetInstance();
		ResourceObjectManager<SkinClusterBase> *skinClusterManager = ResourceObjectManager<SkinClusterBase>::GetInstance();

		result->skinCluster_ = skinClusterManager->Load({ source.assimpHandle_ });

		result->skeletonReference_ = skeletonReferenceManager->Load({ source.assimpHandle_ });

		result->modelInfluence_ = modelInfluenceManager->Load({ source.assimpHandle_ });

		return std::move(result);

	}

}