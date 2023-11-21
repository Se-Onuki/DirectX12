#pragma once
#include <list>
#include "../../../Utils/SoLib/SoLib.h"
#include "../../../Engine/DirectBase/Base/StructBuffer.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"
#include "../Particle.h"

class BlockList;
class IBlock;

/// <summary>
/// ステージ上のブロックをインスタンシングで描画するマネージャ
/// </summary>
class BlockManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	BlockManager() = default;
	~BlockManager() = default;
	BlockManager(const BlockManager&) = delete;
	const BlockManager& operator=(const BlockManager&) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static BlockManager* GetInstance() {
		static BlockManager instance;
		return &instance;
	};

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">最大数</param>
	void Init(uint32_t maxCount);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera3D& camera);

private: // メンバ変数

	// ブロック配列
	ArrayBuffer<Particle::ParticleData> blocks_;

	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// ブロックリスト配列
	std::unordered_map<const Model*, std::unique_ptr<BlockList>> blockMap_;

};

/// <summary>
/// ブロックリスト
/// </summary>
class BlockList {
public: // メンバ関数

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update();

public: // アクセッサ等

	/// <summary>
	/// 粒子配列に粒子を追加する関数
	/// </summary>
	/// <param name="particle">粒子</param>
	/// <returns>配列に追加された粒子</returns>
	IBlock* const push_back(std::unique_ptr<IBlock> particle);

	/// <summary>
	/// 粒子配列のリストゲッター
	/// </summary>
	/// <returns>粒子配列</returns>
	const auto& GetBlockList() { return blocks_; }

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
	void SetModel(const Model* const model) { model_ = model; }

	/// <summary>
	/// 描画インデックス位置セッター
	/// </summary>
	/// <param name="location">描画インデックス位置</param>
	void SetLocation(const uint32_t location) { indexLocation_ = location; }
	/// <summary>
	/// 描画インデックス位置ゲッター
	/// </summary>
	/// <returns>描画インデックス位置</returns>
	const auto& GetLoaction() const { return indexLocation_; }

private: // メンバ変数

	/*==================================
		ユーザー変更禁止（マネージャーで設定）
	==================================*/
	// 描画インデックスの場所
	CBuffer<uint32_t> indexLocation_;

	// ブロックモデル
	const Model* model_;
	// ブロック配列
	std::list<std::unique_ptr<IBlock>> blocks_;
};

class IBlock {
public: // コンストラクタ
	
	/// <summary>
	/// 更新関数
	/// </summary>
	void Update();

public: // パブリックなメンバ変数

	// トランスフォーム
	BaseTransform transform_;

	//　色
	Vector4 color_ = { 1.f, 1.f, 1.f, 1.f };

};