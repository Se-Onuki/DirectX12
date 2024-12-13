/// @file SceneData.h
/// @brief シーンにリソースを紐づけるクラス
/// @author ONUKI seiya
#pragma once
#include <string>			// 文字列
#include <memory>			// メモリ
#include <unordered_map>	// マップ
#include <typeindex>		// 型情報
#include "../DirectBase/Model/ResourceModel.h"

namespace SolEngine {

	namespace SceneManager {

		/// @brief シーンデータ
		class SceneData
		{
		public:
			using ResoureData = ResourceInterface::ResourceMap::MapStrResource::const_iterator;
		public:
			SceneData() = default;
			~SceneData() = default;

		public:




		private:
			// リソースデータ群
			std::unordered_map<std::string, ResourceInterface::ResourceMap> resourceDataMap_;

			// シーン名
			std::string sceneName_;

		};

	}
}