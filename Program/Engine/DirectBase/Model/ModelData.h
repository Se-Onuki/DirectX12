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
#include "MeshInfluence.h"
#include "ModelInfluence.h"

namespace SolEngine {

	class ModelData : public IResourceObject {
	public:
		std::vector<ResourceObjectManager<Mesh>::Handle> meshHandleList_;
		std::unique_ptr<ModelNode> rootNode_;
		ResourceObjectManager<SkinClusterBase>::Handle skinCluster_;
		ResourceObjectManager<SolEngine::SkeletonReference>::Handle skeletonReference_;
		ResourceObjectManager<ModelVertexData>::Handle modelVertex_;
		ResourceObjectManager<ModelInfluence>::Handle modelInfluence_;

		void Draw(const Transform &transform, const Camera3D &camera) const;
		void Draw(const VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> &vertexBuffer, const Transform &transform, const Camera3D &camera) const;
		void Draw(const SkinCluster &skinCluster, const Transform &transform, const Camera3D &camera) const;
		void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const;
		void Draw(const SkinCluster &skinCluster, const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const;
	};

	template <>
	class ResourceSource<ModelData> {
	public:

		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;

		bool operator==(const ResourceSource<ModelData> &) const = default;
	};

	template <>
	class ResourceCreater<ModelData> {
	public:

		std::unique_ptr<ModelData> CreateObject(const ResourceSource<ModelData> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::ModelData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::ModelData> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.assimpHandle_.GetVersion()) };
		}
	};
}