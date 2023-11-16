#pragma once
#include "ParticleManager.h"
#include "../../Entity/Entity.h"

/// <summary>
/// プレイヤーの後ろ方向に向かって星を放出するパーティクル
/// </summary>
class StarParticle : public IParticle
{
public: // メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="emmitionDirection">放出方向</param>
	StarParticle(const Vector3& emissionDirection);

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime) override;



private: // メンバ変数

	// 粒子の放出方向
	const Vector3* emissionDirection_;

	// 演出用t
	float t_;

};

