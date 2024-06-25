#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "AssimpData.h"
#include "../../Utils/Graphics/Color.h"
#include "../Base/CBuffer.h"

struct JointWeightData;

namespace SolEngine {

	class SkinClusterBase : public IResourceObject {
	public:
		std::unordered_map<std::string, JointWeightData> skinClusterData_;
	};

	template <>
	class ResourceSource<SkinClusterBase> {
	public:
		// ファイルのハンドル
		ResourceObjectManager<AssimpData>::Handle assimpHandle_;

		std::string ToStr() const {
			const auto &assimpSource = assimpHandle_.GetSource();
			return assimpSource->directoryPath_ + assimpSource->fileName_;
		}

		bool operator==(const ResourceSource<SkinClusterBase> &) const = default;
	};

	template <>
	class ResourceCreater<SkinClusterBase> {
	public:

		std::unique_ptr<SkinClusterBase> CreateObject(const ResourceSource<SkinClusterBase> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::SkinClusterBase>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::SkinClusterBase> &data) const {
			return size_t{ static_cast<size_t>(data.assimpHandle_.GetHandle()) << 32u | static_cast<size_t>(data.assimpHandle_.GetVersion()) };
		}
	};
}
//
//namespace SoLib {
//	template <>
//	bool ImGuiWidget<SolEngine::Material>(const char *const label, SolEngine::Material *const value);
//
//}
