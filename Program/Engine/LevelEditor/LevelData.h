#pragma once
#include "../ResourceObject/ResourceObject.h"
#include "../../Utils/Math/Transform.h"


namespace SolEngine {

	class LevelData : public IResourceObject {
	public:
		struct ObjectData {
			// すべてキューブ型であるとする

			std::string fileName_;
			std::string name_;
			SimpleTransformEuler transform_;
			std::list<ObjectData> children_;
		};

		std::list<ObjectData> objects_;

	};

	template <>
	class ResourceSource<LevelData> {
	public:
		// ファイルの名前
		std::string fileName_;
	};

	template <>
	class ResourceCreater<LevelData> {

		inline  static const char *const kDefaultDirectory_ = "resources/LevelData/";

	public:

		std::unique_ptr<LevelData> CreateObject(const ResourceSource<LevelData> &source) const;

	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::LevelData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::LevelData> &data) const {
			return std::hash<std::string>{} (data.fileName_);
		}
	};
}