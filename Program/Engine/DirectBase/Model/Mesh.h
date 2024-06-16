#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include <bitset>

#include "AssimpData.h"
#include "../Base/VertexBuffer.h"
#include "Material.h"

namespace SolEngine {

	class Mesh : public IResourceObject {
	public:
		
		struct VertexData {
			Vector4 position; // 頂点位置
			Vector3 normal;   // 法線
			Vector2 texCoord; // UV座標系
		};

		template <uint32_t materialSize>
			requires(materialSize != 0 and materialSize < 8)
		struct VertexTexcoord {
			std::array<Vector2, materialSize> texCoord; // UV座標系

		};
		VertexBuffer<VertexData> vertexBuffer_;
		//VertexBuffer<VertexTexcoord<2>> texcoordBuffer_;
		IndexBuffer<uint32_t> indexBuffer_;
		std::vector<ResourceObjectManager<Material>::Handle> materials_;

		/*decltype(texcoordBuffer_) *GetTexcoordBuffer() { return &texcoordBuffer_; }
		const decltype(texcoordBuffer_) *GetTexcoordBuffer() const { return &texcoordBuffer_; }*/

		ResourceObjectManager<Material>::Handle materialhandle_;

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