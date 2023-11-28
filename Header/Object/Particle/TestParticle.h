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

	void SetStartColor(const Vector4 &color) { startColor_ = color; }
	void SetEndColor(const Vector4 &color) { endColor_ = color; }

private: // メンバ変数

	Vector4 startColor_{ 1.f,1.f,1.f,1.f };
	Vector4 endColor_{ 1.f,1.f,1.f,0.f };

};


