#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

#include "AssimpData.h"
#include "Mesh.h"

#include <d3d12.h>
#include "../Base/CBuffer.h"
#include "../Render/Camera.h"

namespace SolEngine {

	class ModelData : public IResourceObject {
	public:
		std::array<ResourceObjectManager<Mesh>::Handle, AssimpData::kMaterialCountByVertex_> meshHandleList_;
		uint32_t materialCount_ = 0;

		void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const;
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