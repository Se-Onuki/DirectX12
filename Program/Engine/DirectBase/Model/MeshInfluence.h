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

	class MeshInfluence : public IResourceObject {
		using MeshManager = ResourceObjectManager<Mesh>;

	public:
		// 頂点データ
		VertexBuffer<VertexInfluence> influence_;

	};

	template <>
	class ResourceSource<MeshInfluence> {
	public:

		// メッシュのハンドル
		ResourceObjectManager<Mesh>::Handle meshHandle_;
		ResourceObjectManager<SkinClusterBase>::Handle skinClusterBase_;
		ResourceObjectManager<SkeletonReference>::Handle skeletonReference_;

		bool operator==(const ResourceSource<MeshInfluence> &) const = default;
	};

	template <>
	class ResourceCreater<MeshInfluence> {
	public:

		std::unique_ptr<MeshInfluence> CreateObject(const ResourceSource<MeshInfluence> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::MeshInfluence>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::MeshInfluence> &data) const {
			return size_t{ static_cast<size_t>(data.meshHandle_.GetHandle()) << 32u | static_cast<size_t>(data.meshHandle_.GetVersion()) };
		}
	};
}