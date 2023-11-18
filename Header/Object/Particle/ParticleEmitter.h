#pragma once
#include "ParticleManager.h"
#include "../../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../../Utils/SoLib/SoLib.h"

/// <summary>
/// パーティクル生成クラス
/// </summary>
class ParticleEmitter
{
public: // コンストラクタ等

	// コンストラクタ
	ParticleEmitter() = default;
	// デストラクタ
	virtual ~ParticleEmitter() = default;

public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="aliveTime">パーティクル全体の継続時間</param>
	virtual void Init(float aliveTime);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	virtual void Update(float deltaTime);

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// 粒子生成関数
	/// </summary>
	/// <param name="model">粒子モデル</param>
	/// <returns>生成された粒子</returns>
	IParticle* CreateParticle(const Model& model);

	/// <summary>
	/// 生成する粒子の型を設定する関数
	/// </summary>
	/// <typeparam name="SelectParticle">生成する粒子の型</typeparam>
	template<IsIParticle SelectParticle>
	void SetParticleType() {
		type_ = [](const Vector3& position) -> std::unique_ptr<IParticle> {
			return std::make_unique<SelectParticle>(position);
		};
	}

	/// <summary>
	/// 調整項目クラスにメンバ変数を追加する関数
	/// </summary>
	void AddItem();
	/// <summary>
	/// 調整項目クラスにメンバ変数をセットする関数
	/// </summary>
	void SetItem();
	/// <summary>
	/// メンバ変数に調整項目クラスの値をセットする関数
	/// </summary>
	void ApplyItem();

public: // パブリックメンバ変数

#pragma region 調整項目クラスに追加しない変数

	// 発生中心座標
	BaseTransform emitTransform_;

	// 終了しているか
	bool isEnd_ = false;

	// エミッタのタイマー
	SoLib::DeltaTimer emitAliveTimer_;

#pragma endregion

	// 最大粒子数
	int maxParticleCount_ = 10;

	// 粒子発生間隔のタイマー
	SoLib::DeltaTimer emitIntervalTimer_;
	// 粒子の生成間隔
	float emitInterval_ = 1.0f;

	// 粒子の速度ベクトル
	Vector3 particleVelocity_ = Vector3::zero;
	// 粒子の加速度
	Vector3 particleAcceleration_ = Vector3::zero;

	// 生成時のパーティクルのブレ
	float emitBlur_ = 0.0f;

	// 粒子の継続時間
	float particleAliveTme_ = 0.0f;

	// 粒子モデル
	const Model* model_;

	// 生成する粒子の型
	std::function<std::unique_ptr<IParticle>(const Vector3&)> type_;

	// 生成する粒子型名
	std::string typeName_;

#ifdef _DEBUG

	// エミッタの名前
	std::string name_;

#endif // _DEBUG


private: // メンバ変数

	// 調整項目クラス
	GlobalVariables* gv_ = nullptr;

	// パーティクルマネージャ
	ParticleManager* particleManager_ = nullptr;

	// パーティクルリスト
	std::list<IParticle*> particles_;

};

template<class SelectEmiiter>
concept IsIParticleEmitter = std::is_base_of<ParticleEmitter, SelectEmiiter>::value;
