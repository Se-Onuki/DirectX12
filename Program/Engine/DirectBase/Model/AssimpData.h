#pragma once
#include "../../ResourceObject/ResourceObject.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace SolEngine {

	class AssimpData : public IResourceObject {
	public:

		inline static constexpr uint32_t kMaterialCountByVertex_ = 8u;

		// assimpのローダ
		std::unique_ptr<Assimp::Importer> importer_;	// こいつが存在する = シーンデータが保存される
		//aiScene *scene_;

		void Destroy() {
			importer_.reset();
		}

		operator bool() const { return importer_.get(); }
	};

	template <>
	class ResourceSource<AssimpData> {
	public:
		// ディレクトリへのパス
		std::string directoryPath_;
		// ファイルの名前
		std::string fileName_;

		bool operator==(const ResourceSource &) const = default;
	};

	template <>
	class SolEngine::ResourceCreater<AssimpData> {
	public:

		std::unique_ptr<AssimpData> CreateObject(const SolEngine::ResourceSource<AssimpData> &source) const;


	private:


	};
}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::AssimpData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::AssimpData> &data) const {
			return std::hash<std::string>()(data.directoryPath_ + '/' + data.fileName_);
		}
	};
}

