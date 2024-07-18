#pragma once
#include "../ResourceObject/ResourceObject.h"
#include "../ResourceObject/ResourceObjectManager.h"
#include "../../Utils/Math/Transform.h"
#include "../../Header/Collision/Collision.h"
#include "../DirectBase/Model/ModelData.h"


namespace SolEngine {

	class LevelData : public IResourceObject {
	public:
		struct ObjectData {
			// すべてキューブ型であるとする
			//Model *model_; // いったん無視
			ResourceObjectManager<ModelData>::Handle modelHandle_;

			std::string name_;
			SimpleTransformQuaternion transform_;
			VariantShapes collider_ = std::byte{};
			std::list<ObjectData> children_;

			// 接触時に実行される関数名
			std::string triggerFunc_;

		};

		std::list<ObjectData> objectList_;

	};

	template <>
	class ResourceSource<LevelData> {
	public:
		// ファイルの名前
		std::string fileName_;

		bool operator==(const ResourceSource<LevelData> &) const = default;
	};

	template <>
	class ResourceCreater<LevelData> {

		inline static const char *const kDefaultDirectory_ = "resources/LevelData/";

	public:

		std::unique_ptr<LevelData> CreateObject(const ResourceSource<LevelData> &source) const;

	private:

		void RecursiveLoad(const nlohmann::json &jsonObjectList, std::list<LevelData::ObjectData> &objectDataList) const;

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