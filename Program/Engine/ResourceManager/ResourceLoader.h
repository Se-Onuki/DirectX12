/// @file ResourceLoader.h
/// @brief リソースの読み込みを行う
/// @author ONUKI seiya
#pragma once
#include <string>
#include "../Utils/SoLib/SoLib_Json.h"
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

			/// @brief 初期化
			/// @param[in] json 読み込むリソースの情報
			void Init(const Json &json);
			/// @brief ロードの実行
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
