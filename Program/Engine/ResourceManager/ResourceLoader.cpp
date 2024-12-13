/// @file ResourceLoader.cpp
/// @brief リソースの読み込みを行う
/// @author ONUKI seiya
#include "ResourceLoader.h"

namespace SolEngine {

	namespace Resource {

		void ResourceLoadManager::Init(const Json &json)
		{
			// 読み込むリソースの数
			const size_t resourceCount = json.size();
			resourceStructList_.resize(resourceCount);


			std::transform(json.begin(), json.end(), resourceStructList_.begin(), [](const Json &item) ->ResourceStruct
				{
					ResourceStruct result;

					result.resourceName_ = item["Name"];
					result.resourceType_ = item["Type"];
					if (auto itr = item.find("Path"); itr != item.end()) { result.file_path_ = *itr; }
					if (auto itr = item.find("References"); itr != item.end()) { result.references_ = *itr; }

					return result;
				}
			);


			// リソースのソート
			SortStruct();
			// 初期化が終わったというフラグを立てる
			isInitialized_ = true;
		}

		void ResourceLoadManager::Load()
		{
			for (const auto &resStru : resourceStructList_) {
				// 書き込み先のタイプを指定
				auto &targetType = resourceMap_[resStru.resourceType_];
				// フォルダ名がなかったら終わるように
				if (resStru.file_path_.empty() or resStru.resourceType_ != "Model") { continue; }

				// 一回モデルだけを読み込むようにする
				ResourceInterface::ResourceLoader<Model> loader;
				targetType.AddResource(loader.Generate(resStru));
			}

		}

		void ResourceLoadManager::SortStruct()
		{
			// ソートを行い､データを格納する｡
			resourceStructList_ = RecursiveSort().first;
		}

		std::pair<std::list<ResourceStruct>, std::unordered_set<std::string>> &&ResourceLoadManager::RecursiveSort(std::pair<std::list<ResourceStruct>, std::unordered_set<std::string>> &&data)
		{
			// もし､元のデータが消えているなら終わる
			if (resourceStructList_.empty()) { return std::move(data); }

			// もし転送先のデータが空っぽなら真
			const bool isEmpty = data.first.empty();
			// すべてのデータを回す
			for (auto itr = resourceStructList_.begin(); itr != resourceStructList_.end();) {
				// 空っぽで､そのデータに参照データがなかった場合
				if (isEmpty and itr->references_.empty()) {
					data.second.insert(itr->resourceName_);		// 名前を挿入して
					data.first.push_back(std::move(*itr));	// データを転送する
					itr = resourceStructList_.erase(itr);			// イテレータからデータを飛ばす

					continue;	// 次に進む
				}
				bool isRefExist = true;
				// そうでないなら
				// 参照データが全て存在しているかを検索する
				for (const std::string &ref : itr->references_) {
					// もし存在していなかったら
					if (not data.second.contains(ref)) {
						isRefExist = false;
						// その場で終わって次のループへ
						break;
					}
				}
				// 全て存在していたら
				if (isRefExist) {
					data.second.insert(itr->resourceName_);		// 名前を挿入して
					data.first.push_back(std::move(*itr));	// データを転送する
					itr = resourceStructList_.erase(itr);			// イテレータからデータを飛ばす
				}
				else {
					itr++;
				}
			}

			// 再帰的にソートを行う
			return std::move(RecursiveSort(std::move(data)));
		}

	}
}