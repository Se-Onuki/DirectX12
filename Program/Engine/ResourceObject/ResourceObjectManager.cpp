#include "ResourceObjectManager.h"

namespace SolEngine {

	ResourceObjectManager::Handle ResourceObjectManager::Load(const CreateSource &file_path)
	{
		// データを格納する
		Handle result = Find(file_path);
		// すでにデータが存在する場合はそれを返す
		if (result) { return result; }

		// 引数からシェーダを構築
		std::unique_ptr<Shader> shader = std::make_unique<Shader>(Shader::Compile(file_path.name, file_path.profile.c_str()));

		// 構築したデータを格納
		resources_.push_back(std::move(shader));

		// 添え字を代入
		result = static_cast<uint32_t>(resources_.size() - 1);

		// 検索用に保存
		findMap_.insert({ file_path, result });

		return result;
	}

	ResourceObjectManager::Handle ResourceObjectManager::Find(const CreateSource &file_path)
	{
		// 検索を行う
		auto itr = findMap_.find(file_path);
		// 見つかったらそれを返す
		if (itr != findMap_.end()) {

			return itr->second;
		}
		else {
			// 見つからなかったら、不正なデータを返す。
			return Handle{ (std::numeric_limits<uint32_t>::max)() };
		}
	}

}