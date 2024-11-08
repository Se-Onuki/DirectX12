#include "ResourceModel.h"
#include "../../ResourceManager/ResourceLoader.h"

namespace SolEngine {

	/// @brief リソースインターフェース
	namespace ResourceInterface {

		void ResourceMap::AddResource(std::unique_ptr<IResource> &&resource)
		{
			// もしリソースデータがなければエラー
			assert(resource and "リソースがnullptrです");
			// リソース名の取得
			const std::string &name = resource->GetName();
			// リソース名がなければエラー
			assert(name.size() and "リソース名が無名です");
			// リソースの検索
			ResourceInstance itr = Find(name);
			// リソースが保存されている場合
			if (itr != resourceMap_.end()) {
				// その場で終わる
				return;
			}
			// 保存されていない場合
			// データを追加する
			resourceMap_.insert({ name, std::move(resource) });

		}

		ResourceMap::ResourceInstance ResourceMap::Find(const std::string &name) const
		{
			return resourceMap_.find(name);
		}
	}
	namespace ResourceInterface {

		std::unique_ptr<Resource::Model> ResourceLoader<Resource::Model>::Generate(const SolEngine::Resource::ResourceStruct &resource)
		{
			// 返すデータ
			std::unique_ptr<Resource::Model> result = std::make_unique<Resource::Model>();

			std::array<std::string, 2u> file_path;
			auto pos = resource.file_path_.rfind('/');
			if (pos == std::string::npos) {
				// "/" が見つからない場合、前半部分を空文字列、後半部分を input 全体にする
				file_path = { "", resource.file_path_ };
			}
			else {
				// "/" の位置で文字列を2つに分割
				std::string firstPart = resource.file_path_.substr(0, pos + 1); // "/" を含む前半部分
				std::string secondPart = resource.file_path_.substr(pos + 1);   // "/" の後の部分

				file_path = { firstPart, secondPart };
			}
			// 一旦モデルマネージャでごまかす

			SolEngine::ResourceObjectManager<AssimpData> *const assimpManager = SolEngine::ResourceObjectManager<AssimpData>::GetInstance();
			SolEngine::ResourceObjectManager<ModelData> *const modelManager = SolEngine::ResourceObjectManager<ModelData>::GetInstance();
			auto assimpData = assimpManager->Load({ file_path[0],file_path[1] });
			result->model_ = modelManager->Load({ assimpData });
			// 名前の設定
			result->SetName(resource.resourceName_);

			// データを返す
			return std::move(result);
		}

	}
}