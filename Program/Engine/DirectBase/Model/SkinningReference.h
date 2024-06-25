#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

#include "AssimpData.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelData.h"

#include <d3d12.h>
#include "../Base/CBuffer.h"
#include "../Render/Camera.h"
#include "SkinClusterBase.h"

namespace SolEngine {

	class SkinningReference : public IResourceObject {
		using MeshManager = ResourceObjectManager<Mesh>;

	public:
		std::vector<std::pair<MeshManager::Handle, VertexBuffer<VertexInfluence>>> influenceList_;
		std::unique_ptr<SkeletonReference> skeletonReference_;

	};

	template <>
	class ResourceSource<SkinningReference> {
	public:
		ResourceSource<SkinningReference>() = default;
		ResourceSource<SkinningReference>(const ResourceSource<SkinningReference> &) = default;
		ResourceSource<SkinningReference>(const ResourceObjectManager<ModelData>::Handle &data) :modelHandle_(data) {}

		// ファイルのハンドル
		ResourceObjectManager<ModelData>::Handle modelHandle_;

		bool operator==(const ResourceSource<SkinningReference> &) const = default;
	};

	template <>
	class ResourceCreater<SkinningReference> {
	public:

		std::unique_ptr<SkinningReference> CreateObject(const ResourceSource<SkinningReference> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::SkinningReference>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::SkinningReference> &data) const {
			return size_t{ static_cast<size_t>(data.modelHandle_.GetHandle()) << 32u | static_cast<size_t>(data.modelHandle_.GetVersion()) };
		}
	};
}