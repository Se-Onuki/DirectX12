#include "AssimpData.h"
#include <assimp/postprocess.h>

namespace SolEngine {

	std::unique_ptr<AssimpData> ResourceCreater<AssimpData>::CreateObject(const ResourceSource<AssimpData> &source) const {

		std::unique_ptr<AssimpData> result;

		result->importer_->ReadFile((source.directoryPath_ + source.fileName_).c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

		return std::move(result);

	}

}