#pragma once
#include "../../ResourceObject/ResourceObject.h"

namespace SolEngine {

	class Mesh : public IResourceObject {

	};

	template <>
	class ResourceSource<Mesh> {
	public:
		// ファイルの名前
		std::string fileName_;
		// meshのIndex
		uint32_t index_;
	};

}

namespace std {

	template<>
	struct hash<SolEngine::ResourceSource<SolEngine::Mesh>> {
		size_t operator()(const SolEngine::ResourceSource<SolEngine::Mesh> &data) const {
			return std::hash<std::string>()(data.fileName_ + std::to_string(data.index_));
		}
	};
}