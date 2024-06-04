#pragma once
#include "../../ResourceObject/ResourceObject.h"

namespace SolEngine {

	class AssimpData : public IResourceObject {

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

		ResourceCreater() {


		}

		std::unique_ptr<AssimpData> CreateObject(const SolEngine::ResourceSource<AssimpData> &source) const { return {}; }


	private:


	};


}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::AssimpData>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::AssimpData> &data) const {
			return std::hash<std::string>()(data.directoryPath_ + data.fileName_);
		}
	};
}

