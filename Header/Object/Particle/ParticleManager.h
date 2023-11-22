#pragma once
#include <list>
#include "../Particle.h"
#include "../../../Engine/DirectBase/Base/StructBuffer.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"
#include "../../../Utils/SoLib/SoLib.h"
#include "../../../Utils/Math/Vector3.h"

class ParticleList;
class IParticle;

/// <summary>
/// パーティクルマネージャー
/// </summary>
class ParticleManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager &) = delete;
	const ParticleManager &operator=(const ParticleManager &) = delete;

public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleManager *GetInstance() {
		static ParticleManager instance;
		return &instance;
	};

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="maxCount">パーティクル最大数</param>
	void Init(uint32_t maxCount);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera3D &camera);

	IParticle *AddParticle(const Model *const modelKey, std::unique_ptr<IParticle> particle);

private: // メンバ変数

	// 粒子配列
	std::unique_ptr<ArrayBuffer<Particle::ParticleData>> particles_;
	// ヒープレンジ
	DescHeap<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>::HeapRange heapRange_;

	// パーティクル配列
	std::unordered_map<const Model *, std::unique_ptr<ParticleList>> particleMap_;

};

/// <summary>
/// 粒子を格納するリスト
/// </summary>
class ParticleList {
public: // メンバ関数

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

public: // アクセッサ等

	/// <summary>
	/// 粒子配列に粒子を追加する関数
	/// </summary>
	/// <param name="particle">粒子</param>
	/// <returns>配列に追加された粒子</returns>
	IParticle *const push_back(std::unique_ptr<IParticle> particle);

	/// <summary>
	/// 粒子配列のリストゲッター
	/// </summary>
	/// <returns>粒子配列</returns>
	const auto &GetParticleList() { return particles_; }

	/// <summary>
	/// 粒子配列サイズゲッター
	/// </summary>
	/// <returns></returns>
	uint32_t size() const { return static_cast<uint32_t>(particles_.size()); }

	/// <summary>
	/// 粒子配列の始端情報ゲッター
	/// </summary>
	/// <returns>粒子配列始端情報</returns>
	auto begin() { return particles_.begin(); }
	/// <summary>
	/// 粒子配列始端情報ゲッター（定数値）
	/// </summary>
	/// <returns>粒子配列始端情報（定数）</returns>
	auto begin() const { return particles_.cbegin(); }

	/// <summary>
	/// 粒子配列終端情報ゲッター
	/// </summary>
	/// <returns>粒子配列終端情報</returns>
	auto end() { return particles_.end(); }
	/// <summary>
	/// 粒子配列終端情報ゲッター(定数値)
	/// </summary>
	/// <returns>粒子配列終端情報(定数)</returns>
	auto end() const { return particles_.cend(); }

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

private:

	/*==================================
		ユーザー変更禁止（マネージャーで設定）
	==================================*/
	CBuffer<uint32_t> indexLocation_;

	// 粒子モデル
	const Model *model_;
	// 粒子配列
	std::list<std::unique_ptr<IParticle>> particles_;
};

/// <summary>
/// 粒子基底クラス
/// </summary>
class IParticle
{
public: // コンストラクタ等

	IParticle(const Vector3 &translate) { transform_.translate = translate; };			// コンストラクタ
	virtual ~IParticle() = default;	// デストラクタ

public: // 純粋仮想関数

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="deltaTime"></param>
	virtual void Update(float deltaTime) = 0;

public: // アクセッサ等

	/// <summary>
	/// トランスフォームのゲッター
	/// </summary>
	/// <returns>トランスフォーム</returns>
	const BaseTransform &GetTransform()const { return transform_; }

	/// <summary>
	/// 色ゲッター
	/// </summary>
	/// <returns>色</returns>
	const Vector4 &GetColor() const { return color_; }

	/// <summary>
	/// 生存状態ゲッター
	/// </summary>
	/// <returns>生存状態</returns>
	const bool &GetisAlive() const { return isAlive_; }

	/// <summary>
	/// 生存秒数セッター
	/// </summary>
	/// <param name="time">秒数</param>
	void SetAliveTime(float time) { timer_.Start(time); }

public: // パブリックメンバ変数
	// トランスフォーム
	BaseTransform transform_;
	// 色
	Vector4 color_ = { 1.f, 1.f, 1.f, 1.f };

	// 速度
	Vector3 velocity_ = Vector3::zero;
	// 加速度
	Vector3 acceleration_ = Vector3::zero;

	// 乱数
	float randomNumber_ = 0.0f;

	// タイマー
	SoLib::DeltaTimer timer_;

	// 生存トリガー
	bool isAlive_ = true;
};

template<class SelectParticle>
concept IsIParticle = std::is_base_of<IParticle, SelectParticle>::value;