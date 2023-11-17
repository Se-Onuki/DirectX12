#pragma once
#include "ParticleManager.h"
#include "../../../Utils/SoLib/SoLib.h"

/// <summary>
/// パーティクル生成基底クラス
/// </summary>
class IParticleEmitter
{
public: // コンストラクタ等

	// コンストラクタ
	IParticleEmitter() = default;
	// デストラクタ
	virtual ~IParticleEmitter() = default;

public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="model">粒子モデル</param>
	/// <param name="aliveTime">パーティクル全体の継続時間</param>
	virtual void Init(Model* model, float aliveTime);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	virtual void Update(float deltaTime) = 0;

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	virtual void DisplayImGui() = 0;

	template<IsIParticle SelectParticle>
	inline SelectParticle* CreateParticle(const Model& model) {
		// 選択されたパーティクルを生成する
		SelectParticle particle = particleManager_->AddParticle(model, std::make_unique<SelectParticle>());
		// 生成したパーティクルにパラメーターを設定
		particle->velocity_ = particleVelocity_;		 // 速度設定
		particle->acceleration_ = particleAcceleration_; // 加速度設定
		particle->randomNumber_ = emitBlur_;			 // 乱数設定
		particle->timer_->Start(particleAliveTme_);		 // 粒子タイマー

		// 生成したパーティクルを返す
		return particle;
	}

public: // パブリックメンバ変数

	// 発生中心座標
	BaseTransform emitTransform_;

	// 終了しているか
	bool isEnd_ = false;

	// 最大粒子数
	int maxParticleCount_ = 10;

	// エミッタのタイマー
	SoLib::DeltaTimer emitAliveTimer_;
	// 粒子の生成間隔
	float emitInterval_ = 0.0f;

	// 粒子の速度ベクトル
	Vector3 particleVelocity_;
	// 粒子の加速度
	Vector3 particleAcceleration_;

	// 生成時のパーティクルのブレ
	float emitBlur_ = 0.0f;

	// 粒子の継続時間
	float particleAliveTme_ = 0.0f;

	// 粒子モデル
	Model* model_;

protected: // 継承先メンバ変数メンバ変数

	// パーティクルマネージャ
	ParticleManager* particleManager_;

	// パーティクルリスト
	std::list<IParticle*> particles_;

};

template<class SelectEmiiter>
concept IsIParticleEmitter = std::is_base_of<IParticleEmitter, SelectEmiiter>::value;
