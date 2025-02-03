/// @file ModelData.h
/// @brief モデルデータ(現行のモデルデータ)
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
#include "SkeletonReference.h"
#include "MeshInfluence.h"
#include "ModelInfluence.h"

namespace SolEngine {

	class ModelData : public IResourceObject {
	public:
		std::vector<ResourceHandle<Mesh>> meshHandleList_;
		ResourceHandle<ModelVertexData> modelVertex_;
		CBuffer<Matrix4x4> rootNode_;

		/// @brief モデルの描画
		void Draw(const SoLib::Transform &transform, const Camera3D &camera) const;
		void Draw(const VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> &vertexBuffer, const SoLib::Transform &transform, const Camera3D &camera) const;
		void Draw(const SkinCluster &skinCluster, const SoLib::Transform &transform, const Camera3D &camera) const;
		void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const;
		void Draw(const SkinCluster &skinCluster, const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const;
	};

	template <>
	class ResourceSource<ModelData> {
	public:

		// ファイルのハンドル
		ResourceHandle<AssimpData> assimpHandle_;

		bool operator==(const ResourceSource<ModelData> &) const = default;
	};

	template <>
	class ResourceCreater<ModelData> {
	public:

		/// @brief リソースの作成
		/// @param[in] source リソースソース
		/// @return 作成したリソース
		std::unique_ptr<ModelData> CreateObject(const ResourceSource<ModelData> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::ModelData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::ModelData> &data) const {
			return data.assimpHandle_.GetHashID();
		}
	};
}