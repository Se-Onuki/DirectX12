#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include <bitset>

#include "AssimpData.h"
#include "../Base/VertexBuffer.h"
#include "Material.h"
#include "ModelVertexData.h"

namespace SolEngine {

	class Mesh : public IResourceObject {
	public:

		// 頂点データハンドル
		const ModelVertexData *modelVertex_;
		// MeshのIndex
		uint32_t meshIndex_;

		// マテリアルハンドル
		ResourceObjectManager<Material>::Handle materialhandle_;

		void Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount, const VertexBuffer<ModelVertexData::VertexData, D3D12_HEAP_TYPE_DEFAULT> &vertex) const;
		void Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount = 1u, const D3D12_VERTEX_BUFFER_VIEW *vbv = nullptr) const;
	};

	template <>
	class ResourceSource<Mesh> {
	public:
		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;
		// meshのIndex
		uint32_t index_;

		bool operator==(const ResourceSource<Mesh> &) const = default;
	};

	template <>
	class ResourceCreater<Mesh> {
	public:

		std::unique_ptr<Mesh> CreateObject(const ResourceSource<Mesh> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Mesh>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Mesh> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.index_) };
		}
	};
}