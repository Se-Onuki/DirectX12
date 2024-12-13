/// @file MeshInfluence.h
/// @brief メッシュへの影響を保存する
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
		// スキンクラスタのハンドル
		ResourceObjectManager<SkinClusterBase>::Handle skinClusterBase_;
		// スケルトンのハンドル
		ResourceObjectManager<SkeletonJointReference>::Handle skeletonReference_;

		bool operator==(const ResourceSource<MeshInfluence> &) const = default;
	};

	template <>
	class ResourceCreater<MeshInfluence> {
	public:
		/// @brief リソースを作成する
		/// @param[in] source リソースソース
		/// @return 作成したリソース
		std::unique_ptr<MeshInfluence> CreateObject(const ResourceSource<MeshInfluence> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::MeshInfluence>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::MeshInfluence> &data) const {
			return data.meshHandle_.GetHashID();
		}
	};
}