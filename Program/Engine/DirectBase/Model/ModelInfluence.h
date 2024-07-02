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
		using MeshManager = ResourceObjectManager<Mesh>;

	public:
		// 頂点データ
		ArrayBuffer<VertexInfluence> influence_;

		std::vector<D3D12_VERTEX_BUFFER_VIEW> vbvs_;

	};

	template <>
	class ResourceSource<ModelInfluence> {
	public:

		ModelData *modelData_;

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
			return size_t{ reinterpret_cast<size_t>(data.modelData_) };
		}
	};
}