/// @file M<odelInfluence.h
/// @brief モデルの影響情報
/// @author ONUKI seiya
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

	public:
		// 頂点データ
		VertexBuffer<VertexInfluence> influence_;
		DescHeapCbvSrvUav::HeapRange heapRange_;

		// 頂点数
		CBuffer<uint32_t> vertexCount_;

	};

	template <>
	class ResourceSource<ModelInfluence> {
	public:
		ResourceSource<ModelInfluence>() = default;
		ResourceSource<ModelInfluence>(const ResourceSource<ModelInfluence> &) = default;
		ResourceSource<ModelInfluence> &operator=(const ResourceSource<ModelInfluence> &) = default;
		ResourceSource<ModelInfluence>(const ResourceHandle<AssimpData> assimpHandle) : assimpHandle_(assimpHandle) {}

		ResourceHandle<AssimpData> assimpHandle_;

		bool operator==(const ResourceSource<ModelInfluence> &) const = default;
	};

	template <>
	class ResourceCreater<ModelInfluence> {
	public:

		/// @brief リソースの作成
		/// @param[in] source リソースソース
		/// @return 作成したリソース
		std::unique_ptr<ModelInfluence> CreateObject(const ResourceSource<ModelInfluence> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::ModelInfluence>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::ModelInfluence> &data) const {
			return data.assimpHandle_.GetHashID();
		}
	};
}