#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include <bitset>

#include "AssimpData.h"
#include "../Base/VertexBuffer.h"

namespace SolEngine {

	class ModelVertexData : public IResourceObject {
	public:
		/// @brief 頂点データ
		struct VertexData {
			Vector4 position; // 頂点位置
			Vector3 normal;   // 法線
			Vector2 texCoord; // UV座標系
		};

		struct VertexOffset {
			uint32_t vertexOffset_;
			uint32_t indexOffset_;
			uint32_t vertexCount_;
			uint32_t indexCount_;

			template<typename T>
			std::span<T> GetMeshSpan(T *begin) const { return std::span<T>{ begin + vertexOffset_, vertexCount_ }; }
			std::span<uint32_t> GetIndexSpan(uint32_t *begin) const { return std::span<uint32_t>{ begin + indexOffset_, indexCount_ }; }

		};

		// 頂点データ
		VertexBuffer<VertexData> vertexBuffer_;
		// index情報
		IndexBuffer<uint32_t> indexBuffer_;

		// メッシュの分解情報
		std::vector<VertexOffset> vertexOffsets_;

	};

	template <>
	class ResourceSource<ModelVertexData> {
	public:
		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;

		bool operator==(const ResourceSource<ModelVertexData> &) const = default;
	};

	template <>
	class ResourceCreater<ModelVertexData> {
	public:

		std::unique_ptr<ModelVertexData> CreateObject(const ResourceSource<ModelVertexData> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::ModelVertexData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::ModelVertexData> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.assimpHandle_.GetVersion()) };
		}
	};
}