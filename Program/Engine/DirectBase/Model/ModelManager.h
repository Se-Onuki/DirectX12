#pragma once
#include <string>
#include <unordered_map>

#include "Model.h"

using ModelHash = size_t;

class ModelManager {
public:
	static ModelManager *const GetInstance() {
		static ModelManager instance;
		return &instance;
	}

	/// @brief 簡易的なモデルを作る
	void CreateDefaultModel();

	/// @brief モデルの追加
	/// @param key 文字列キー
	/// @param model モデルデータ
	Model *const AddModel(const std::string &key, std::unique_ptr<Model> model) {
		// モデルを検索して、存在したらそれを返す
		auto modelPtr = GetModel(key);
		if (modelPtr) { return modelPtr; }

		models_[key] = std::move(model);
		return models_[key].get();
	}

	/// @brief モデルデータの取得
	/// @param key 文字列キー
	/// @return モデルデータ
	Model *const GetModel(const std::string &key) const {
		auto item = models_.find(key);
		if (item != models_.end()) {
			return item->second.get();
		}
		return nullptr;
	}

	/// @brief モデルの破棄
	/// @param key 文字列キー
	void RemoveModel(const std::string &key) {
		// 全ての要素から検索
		const auto &it = models_.find(key);
		// 無かったらその時点で終了
		if (it == models_.end()) return;
		// 存在したら消去
		models_.erase(it);
	}

	/// @brief 全てのモデルの破棄
	void RemoveAll() {
		models_.clear();
	}

private:
	ModelManager() = default;
	ModelManager(const ModelManager &) = delete;
	ModelManager &operator=(const ModelManager &) = delete;
	~ModelManager() = default;

	// モデルマップ
	std::unordered_map<std::string, std::unique_ptr<Model>> models_;

private:
};