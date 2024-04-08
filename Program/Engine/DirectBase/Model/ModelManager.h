#pragma once
#include <string>
#include <unordered_map>

#include "Model.h"
#include "../../Utils/Containers/Singleton.h"

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

class MeshManager : public SoLib::Singleton<MeshManager>
{
	friend SoLib::Singleton<MeshManager>;
	MeshManager() = default;
	MeshManager(const MeshManager &) = delete;
	MeshManager &operator=(const MeshManager &) = delete;
	~MeshManager() = default;

public:

	class Key {
	public:
		Key() = default;
		Key(uint32_t v) :index_(v) {}

		operator uint32_t () const noexcept { return index_; }
		uint32_t operator=(const uint32_t v) noexcept { return index_ = v; }

		Mesh *const operator->()const { return pMeshManager_->InnerGetMesh(index_); }

		// 有効な値を持っているか
		operator bool() const noexcept {
			return
				// 範囲外に出てたら無効
				pMeshManager_->meshArray_.size() > index_
				// データが存在しない場合は無効
				and pMeshManager_->meshArray_.at(index_);
		}

		uint32_t GetIndex() const noexcept { return index_; }

	private:
		static MeshManager *const pMeshManager_;

		uint32_t index_ = (std::numeric_limits<uint32_t>::max)();

	};
	/// @brief メッシュの追加
	/// @param mesh メッシュデータ
	/// @return メッシュのキー
	Key AddMesh(std::unique_ptr<Mesh> mesh);

	/// @brief メッシュの取得
	/// @param key メッシュキー
	/// @return メッシュ
	Mesh *const GetMesh(const Key key) const {

		return InnerGetMesh(key.GetIndex());
	}

	/// @brief そのキーが有効か
	/// @param key キー
	/// @return 有効な値ならtrue
	bool IsActiveKey(const Key key) const {
		return
			// 範囲外に出てたら無効
			meshArray_.size() > key.GetIndex()
			// データが存在しない場合は無効
			and meshArray_.at(key.GetIndex());
	}

private:


	Mesh *const InnerGetMesh(const uint32_t index) const {
		// 頻度は少ないが、存在しない場合はnullptrを返す
		if (meshArray_.size() < index) [[unlikely]] { return nullptr; }

		return meshArray_[index].get();
	}

	std::vector<std::unique_ptr<Mesh>> meshArray_;

};