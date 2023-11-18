#pragma once
#include "ParticleManager.h"

/// <summary>
/// テストパーティクル
/// </summary>
class TestParticle : public IParticle
{
public: // メンバ関数

	// 基底クラスのコンストラクタを使用
	using IParticle::IParticle;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime) override;

private: // メンバ変数

	

};


