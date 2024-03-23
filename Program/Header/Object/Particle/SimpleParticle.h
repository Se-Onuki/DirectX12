#pragma once
#include "ParticleManager.h"

/// <summary>
/// テストパーティクル
/// </summary>
class SimpleParticle : public IParticle
{
public: // 静的なメンバ関数

	static void AddItem();

public: // メンバ関数

	// 基底クラスのコンストラクタを使用
	using IParticle::IParticle;

	/// @brief ビルボード計算を行うか
	/// @return 行わない
	bool IsBillboard() const override { return false; }

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


