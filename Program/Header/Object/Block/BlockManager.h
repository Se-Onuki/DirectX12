#pragma once
#include <list>
#include "../../Engine/Utils/SoLib/SoLib.h"
#include "../../../Engine/DirectBase/Base/StructBuffer.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"
#include "../Particle.h"
#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/Containers/Singleton.h"
#include "../../../Engine/DirectBase/Model/ModelData.h"

class IBlock {
public: // コンストラクタ

public: // パブリックなメンバ変数

	// トランスフォーム
	Matrix4x4 transMat_;

	//　色
	SoLib::Color::RGB4 color_ = 0xFFFFFFFF;

};
/// <summary>
/// ブロックリスト
/// </summary>
class BlockList {
public: // メンバ関数


public: // アクセッサ等

	void clear() { blocks_.clear(); }
	/// <summary>
	/// 粒子配列に粒子を追加する関数
	/// </summary>
	/// <param name="particle">粒子</param>
	/// <returns>配列に追加された粒子</returns>
	IBlock *const push_back(IBlock &&block);

	/// <summary>
	/// 粒子配列のリストゲッター
	/// </summary>
	/// <returns>粒子配列</returns>
	const auto &GetBlockList() { return blocks_; }

	/// <summary>
	/// 粒子配列サイズゲッター
	/// </summary>
	/// <returns></returns>
	uint32_t size() const { return static_cast<uint32_t>(blocks_.size()); }

	/// <summary>
	/// 粒子配列の始端情報ゲッター
	/// </summary>
	/// <returns>粒子配列始端情報</returns>
	auto begin() { return blocks_.begin(); }
	/// <summary>
	/// 粒子配列始端情報ゲッター（定数値）
	/// </summary>
	/// <returns>粒子配列始端情報（定数）</returns>
	auto begin() const { return blocks_.cbegin(); }

	/// <summary>
	/// 粒子配列終端情報ゲッター
	/// </summary>
	/// <returns>粒子配列終端情報</returns>
	auto end() { return blocks_.end(); }
	/// <summary>
	/// 粒子配列終端情報ゲッター(定数値)
	/// </summary>
	/// <returns>粒子配列終端情報(定数)</returns>
	auto end() const { return blocks_.cend(); }

	/// <summary>
	/// パーティクルに使用するモデルセッター
	/// </summary>
	/// <param name="model">パーティクルに使用するモデル</param>
	void SetModel(const Model *const model) { model_ = model; }

	/// <summary>
	/// 描画インデックス位置セッター
	/// </summary>
	/// <param name="location">描画インデックス位置</param>
	void SetLocation(const uint32_t location) { indexLocation_ = location; }
	/// <summary>
	/// 描画インデックス位置ゲッター
	/// </summary>
	/// <returns>描画インデックス位置</returns>
	const auto &GetLoaction() const { return indexLocation_; }

private: // メンバ変数

	/*==================================
		ユーザー変更禁止（マネージャーで設定）
	==================================*/
	// 描画インデックスの場所
	CBuffer<uint32_t> indexLocation_;

	// ブロックモデル
	const Model *model_ = nullptr;
	// ブロック配列
	std::list<IBlock> blocks_{};
};

/// <summary>
/// ステージ上のブロックをインスタンシングで描画するマネージャ
/// </summary>
class BlockManager : public SoLib::Singleton<BlockManager> {
private: // コンストラクタ等

	// シングルトンパターンの設定
	BlockManager() = default;
	~BlockManager() = default;
	BlockManager(const BlockManager &) = delete;
	const BlockManager &operator=(const BlockManager &) = delete;

public: // メンバ関数
	friend SoLib::Singleton<BlockManager>;

	/*/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static BlockManager *GetInstance() {
		static BlockManager instance;
		return &instance;
	};*/

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">最大数</param>
	void Init(uint32_t maxCount);


	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera3D &camera);
	void Draw(const CBuffer<Camera3D::CameraMatrix> &camera);

	IBlock *const AddBox(const Model *const model, IBlock &&block) {

		if (not model) { return nullptr; }

		decltype(blockMap_)::iterator blockListItr = blockMap_.find(model);
		if (blockListItr == blockMap_.end()) {
			blockMap_[model] = std::make_unique<BlockList>();
			blockMap_[model]->SetModel(model);
		}
		return blockMap_[model]->push_back(std::move(block));
	}

	/// @brief 破棄関数
	void clear() {
		for (auto &blockList : blockMap_) {
			blockList.second->clear();
		}
	}

private: // メンバ変数

	// ブロック配列
	ArrayBuffer<Particle::ParticleData> blocks_;

	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// ブロックリスト配列
	std::unordered_map<const Model *, std::unique_ptr<BlockList>> blockMap_;

};

/// <summary>
/// ステージ上のブロックをインスタンシングで描画するマネージャ
/// </summary>
class SkinModelListManager : public SoLib::Singleton<SkinModelListManager> {
private: // コンストラクタ等

	// シングルトンパターンの設定
	SkinModelListManager() = default;
	~SkinModelListManager() = default;
	SkinModelListManager(const SkinModelListManager &) = delete;
	const SkinModelListManager &operator=(const SkinModelListManager &) = delete;

public: // メンバ関数
	friend SoLib::Singleton<SkinModelListManager>;

	using SkinModelPair = std::pair<const Model *, const SkinModel *>;

	struct hash {
		size_t operator()(const SkinModelPair &data) const {
			size_t result{};

			result = std::hash<decltype(data.first)>{}(data.first) + std::hash<decltype(data.second)>{}(data.second);

			return result;
		}
	};

	/*/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static BlockManager *GetInstance() {
		static BlockManager instance;
		return &instance;
	};*/

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">最大数</param>
	void Init(uint32_t maxCount);


	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera3D &camera);
	//void Draw(const CBuffer<Camera3D::CameraMatrix> &camera);

	IBlock *const AddBox(const SkinModelPair &model, IBlock &&block) {

		if (not model.first or not model.second) { return nullptr; }

		decltype(blockMap_)::iterator blockListItr = blockMap_.find(model);
		if (blockListItr == blockMap_.end()) {
			blockMap_[model] = std::make_unique<BlockList>();
			blockMap_[model]->SetModel(model.first);
		}
		return blockMap_[model]->push_back(std::move(block));
	}

	/// @brief 破棄関数
	void clear() {
		for (auto &blockList : blockMap_) {
			blockList.second->clear();
		}
	}

private: // メンバ変数

	// ブロック配列
	ArrayBuffer<Particle::ParticleData> blocks_;

	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// ブロックリスト配列
	std::unordered_map<SkinModelPair, std::unique_ptr<BlockList>, hash> blockMap_;

};

/// <summary>
/// ブロックリスト
/// </summary>
class ModelHandleDrawList {
	using ModelHandle = SolEngine::ResourceObjectManager<SolEngine::ModelData>::Handle;

public: // メンバ関数


public: // アクセッサ等

	void clear() { blocks_.clear(); }
	/// <summary>
	/// 粒子配列に粒子を追加する関数
	/// </summary>
	/// <param name="particle">粒子</param>
	/// <returns>配列に追加された粒子</returns>
	IBlock *const push_back(IBlock &&block);

	/// <summary>
	/// 粒子配列のリストゲッター
	/// </summary>
	/// <returns>粒子配列</returns>
	const auto &GetBlockList() { return blocks_; }

	/// <summary>
	/// 粒子配列サイズゲッター
	/// </summary>
	/// <returns></returns>
	uint32_t size() const { return static_cast<uint32_t>(blocks_.size()); }

	/// <summary>
	/// 粒子配列の始端情報ゲッター
	/// </summary>
	/// <returns>粒子配列始端情報</returns>
	auto begin() { return blocks_.begin(); }
	/// <summary>
	/// 粒子配列始端情報ゲッター（定数値）
	/// </summary>
	/// <returns>粒子配列始端情報（定数）</returns>
	auto begin() const { return blocks_.cbegin(); }

	/// <summary>
	/// 粒子配列終端情報ゲッター
	/// </summary>
	/// <returns>粒子配列終端情報</returns>
	auto end() { return blocks_.end(); }
	/// <summary>
	/// 粒子配列終端情報ゲッター(定数値)
	/// </summary>
	/// <returns>粒子配列終端情報(定数)</returns>
	auto end() const { return blocks_.cend(); }

	/// <summary>
	/// パーティクルに使用するモデルセッター
	/// </summary>
	/// <param name="model">パーティクルに使用するモデル</param>
	void SetModel(const ModelHandle model) { model_ = model; }

	/// <summary>
	/// 描画インデックス位置セッター
	/// </summary>
	/// <param name="location">描画インデックス位置</param>
	void SetLocation(const uint32_t location) { indexLocation_ = location; }
	/// <summary>
	/// 描画インデックス位置ゲッター
	/// </summary>
	/// <returns>描画インデックス位置</returns>
	const auto &GetLoaction() const { return indexLocation_; }

private: // メンバ変数

	/*==================================
		ユーザー変更禁止（マネージャーで設定）
	==================================*/
	// 描画インデックスの場所
	CBuffer<uint32_t> indexLocation_;

	// ブロックモデル
	ModelHandle model_;
	// ブロック配列
	std::list<IBlock> blocks_;
};

/// <summary>
/// ステージ上のブロックをインスタンシングで描画するマネージャ
/// </summary>
class ModelHandleListManager : public SoLib::Singleton<ModelHandleListManager> {
private: // コンストラクタ等

	// シングルトンパターンの設定
	ModelHandleListManager() = default;
	~ModelHandleListManager() = default;
	ModelHandleListManager(const ModelHandleListManager &) = delete;
	const ModelHandleListManager &operator=(const ModelHandleListManager &) = delete;

	friend SoLib::Singleton<ModelHandleListManager>;

	using ModelHandle = SolEngine::ResourceObjectManager<SolEngine::ModelData>::Handle;

public: // メンバ関数

	struct hash {
		size_t operator()(const SolEngine::ResourceObjectManager<SolEngine::ModelData>::Handle data) const {
			size_t result{};

			result = data.GetVersion();
			result <<= 32;
			result += data.GetHandle();

			return result;
		}
	};


	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">最大数</param>
	void Init(uint32_t maxCount);


	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera3D &camera);
	//void Draw(const CBuffer<Camera3D::CameraMatrix> &camera);

	IBlock *const AddBox(const ModelHandle model, IBlock &&block) {

		if (not model) { return nullptr; }

		decltype(blockMap_)::iterator blockListItr = blockMap_.find(model);
		if (blockListItr == blockMap_.end()) {
			blockMap_[model] = std::make_unique<ModelHandleDrawList>();
			blockMap_[model]->SetModel(model);
		}
		return blockMap_[model]->push_back(std::move(block));
	}

	/// @brief 破棄関数
	void clear() {
		for (auto &blockList : blockMap_) {
			blockList.second->clear();
		}
	}

private: // メンバ変数

	// ブロック配列
	ArrayBuffer<Particle::ParticleData> blocks_;

	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// ブロックリスト配列
	std::unordered_map<ModelHandle, std::unique_ptr<ModelHandleDrawList>, hash> blockMap_;

};

/// <summary>
/// ステージ上のブロックをインスタンシングで描画するマネージャ
/// </summary>
class SkinModelHandleListManager : public SoLib::Singleton<SkinModelHandleListManager> {
private: // コンストラクタ等

	// シングルトンパターンの設定
	SkinModelHandleListManager() = default;
	~SkinModelHandleListManager() = default;
	SkinModelHandleListManager(const SkinModelHandleListManager &) = delete;
	const SkinModelHandleListManager &operator=(const SkinModelHandleListManager &) = delete;

	friend SoLib::Singleton<SkinModelHandleListManager>;

	using ModelHandle = SolEngine::ResourceObjectManager<SolEngine::ModelData>::Handle;

public: // メンバ関数

	using SkinModelPair = std::pair<ModelHandle, const SkinModel *>;

	struct hash {
		inline size_t operator()(const ModelHandle data) const {
			size_t result{};

			result = data.GetVersion();
			result <<= 32;
			result += data.GetHandle();

			return result;
		}
		inline size_t operator()(const SkinModelPair &data) const {
			size_t result{};

			result = std::hash<size_t>{}(hash{}(data.first) + reinterpret_cast<size_t>(data.second));

			return result;
		}
	};

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">最大数</param>
	void Init(uint32_t maxCount);


	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera3D &camera);
	//void Draw(const CBuffer<Camera3D::CameraMatrix> &camera);

	IBlock *const AddBox(const SkinModelPair &model, IBlock &&block) {

		if (not model.first) { return nullptr; }

		decltype(blockMap_)::iterator blockListItr = blockMap_.find(model);
		if (blockListItr == blockMap_.end()) {
			blockMap_[model] = std::make_unique<ModelHandleDrawList>();
			blockMap_[model]->SetModel(model.first);
		}
		return blockMap_[model]->push_back(std::move(block));
	}

	/// @brief 破棄関数
	void clear() {
		for (auto &blockList : blockMap_) {
			blockList.second->clear();
		}
	}

private: // メンバ変数

	// ブロック配列
	ArrayBuffer<Particle::ParticleData> blocks_;

	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// ブロックリスト配列
	std::unordered_map<SkinModelPair, std::unique_ptr<ModelHandleDrawList>, hash> blockMap_;

};

