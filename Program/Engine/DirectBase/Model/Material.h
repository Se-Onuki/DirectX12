#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "AssimpData.h"
#include "../../Utils/Graphics/Color.h"
#include "../Base/CBuffer.h"
#include "Model.h"

namespace SolEngine {

	class Material : public IResourceObject {
	public:
		struct MaterialData {
			SoLib::Color::RGB4 color;		// 色(RGBA)
			SoLib::Color::RGB4 emissive;    // 自己発光色(RGBA)
			Matrix4x4 uvTransform;

			float shininess = { 1.f };
		};

		CBuffer<MaterialData> materialData_;
		Model::BlendMode blendMode_ = Model::BlendMode::kNone;

		uint32_t texHandle_;
	};

	template <>
	class ResourceSource<Material> {
	public:
		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;
		// materialのmodel内Index
		uint32_t index_;

		bool operator==(const ResourceSource<Material> &) const = default;
	};

	template <>
	class ResourceCreater<Material> {
	public:

		std::unique_ptr<Material> CreateObject(const ResourceSource<Material> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Material>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Material> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.index_) };
		}
	};
}