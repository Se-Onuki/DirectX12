#include "AssimpData.h"
#include <assimp/postprocess.h>

namespace SolEngine {

	std::unique_ptr<AssimpData> ResourceCreater<AssimpData>::CreateObject(const ResourceSource<AssimpData> &source) const {

		std::unique_ptr<AssimpData> result = std::make_unique<AssimpData>();
		result->importer_ = std::make_unique<Assimp::Importer>();

		result->importer_->ReadFile(("resources/" + source.directoryPath_ + source.fileName_).c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

		return std::move(result);

	}

}