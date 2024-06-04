#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "AssimpData.h"
#include "../Base/VertexBuffer.h"

namespace SolEngine {

	class Mesh : public IResourceObject {
		struct VertexData {
			Vector4 position; // 頂点位置
			Vector2 texCoord; // UV座標系
			Vector3 normal;   // 法線
		};

		VertexBuffer<VertexData> vertexBuffer_;
		IndexBuffer<uint32_t> indexBuffer_;
	};

	template <>
	class ResourceSource<Mesh> {
	public:
		// ファイルの名前
		AssimpDataManager::Handle fileName_;
		// meshのIndex
		uint32_t index_;
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
			return size_t{ static_cast<size_t>(data.fileName_.GetHandle()) << 32 | static_cast<size_t>(data.index_) };
		}
	};
}