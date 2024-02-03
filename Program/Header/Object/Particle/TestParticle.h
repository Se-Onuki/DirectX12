#pragma once
#include "ParticleManager.h"

/// <summary>
/// テストパーティクル
/// </summary>
class TestParticle : public IParticle
{
public: // 静的なメンバ関数

	static void AddItem();

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

	Vector3 startScale_ = Vector3::one * 5.f;
	Vector3 endScale_{};

private: // メンバ変数



};


