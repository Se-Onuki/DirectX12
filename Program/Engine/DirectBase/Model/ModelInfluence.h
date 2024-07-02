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
#include "SkeletonReference.h"

namespace SolEngine {

	class ModelData;

	class ModelInfluence : public IResourceObject {
		//using ModelManager = ResourceObjectManager<Model>;

	public:
		// 頂点データ
		VertexBuffer<VertexInfluence> influence_;
		DescHeapCbvSrvUav::HeapRange heapRange_;


	};

	template <>
	class ResourceSource<ModelInfluence> {
	public:

		ResourceObjectManager<AssimpData>::Handle assimpData_;
		ResourceObjectManager<SkinClusterBase>::Handle skinClusterBase_;
		ResourceObjectManager<SkeletonReference>::Handle skeletonReference_;

		bool operator==(const ResourceSource<ModelInfluence> &) const = default;
	};

	template <>
	class ResourceCreater<ModelInfluence> {
	public:

		std::unique_ptr<ModelInfluence> CreateObject(const ResourceSource<ModelInfluence> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::ModelInfluence>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::ModelInfluence> &data) const {
			return size_t{ static_cast<size_t>(data.assimpData_.GetHandle()) << 32u | static_cast<size_t>(data.assimpData_.GetVersion()) };
		}
	};
}