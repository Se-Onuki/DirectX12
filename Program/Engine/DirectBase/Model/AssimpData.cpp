/// @file AssimpData.cpp
/// @brief Assimpのデータの保持
/// @author ONUKI seiya
#include "AssimpData.h"
#include <assimp/postprocess.h>

namespace SolEngine {

	std::unique_ptr<AssimpData> ResourceCreater<AssimpData>::CreateObject(const ResourceSource<AssimpData> &source) const {

		std::unique_ptr<AssimpData> result = std::make_unique<AssimpData>();

		result->importer_ = std::make_unique<Assimp::Importer>();

		// 読み込み時に適用する設定
		constexpr aiPostProcessSteps kAiProcess = static_cast<aiPostProcessSteps>(
			aiProcess_FlipWindingOrder	// 頂点の回転方向を逆にする
			| aiProcess_FlipUVs			// UVを反転する
			| aiProcess_Triangulate		// 必ず三角面として扱う
			);

		// ファイルを読み込む｡読み込みに失敗したら無効なデータを渡す｡
		if (not result->importer_->ReadFile(("resources/" + source.directoryPath_ + source.fileName_).c_str(), kAiProcess)) {
			return nullptr;
		}

		return std::move(result);

	}

}