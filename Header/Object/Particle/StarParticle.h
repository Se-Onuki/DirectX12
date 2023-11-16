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
	/// <param name="translate">プレイヤー座標</param>
	/// <param name="direction">プレイヤーの向き</param>
	StarParticle(const Vector3& translate,  const Vector3& direction);

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

	

};

