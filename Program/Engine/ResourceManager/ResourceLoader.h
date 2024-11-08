#pragma once
#include <string>
#include "../nlohmann/json.hpp"
#include "../Utils/IO/File.h"
#include <unordered_set>
#include "../DirectBase/Model/ResourceModel.h"

namespace SolEngine {

	namespace Resource {

		/// @brief リソースの情報を保存する
		struct ResourceStruct {
			// リソースデータの名前
			std::string resourceName_;
			// リソースタイプ
			std::string resourceType_;
			// ファイルパス
			std::string file_path_;
			// 参照リソース
			std::vector<std::string> references_;
		};

		class ResourceLoadManager
		{
		private:
			using Json = nlohmann::json;
			using File = SoLib::IO::File;
		public:

		public:
			ResourceLoadManager() = default;
			~ResourceLoadManager() = default;

			void Init(const Json &json);

			void Load();

		private:

			/// @brief データ群のソート
			void SortStruct();

			std::pair<std::list<ResourceStruct>, std::unordered_set<std::string>> &&RecursiveSort(std::pair<std::list<ResourceStruct>, std::unordered_set<std::string>> &&data = {});

		private:
			// リソースのデータ群
			std::list<ResourceStruct> resourceStructList_;

			// リソースデータ群
			std::unordered_map<std::string, SolEngine::ResourceInterface::ResourceMap> resourceMap_;

			// 初期化が終わっているか
			bool isInitialized_ = false;

		};
	}

}
