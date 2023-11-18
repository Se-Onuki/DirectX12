#pragma once
#include "ParticleManager.h"
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Entity/Entity.h"

/// <summary>
/// プレイヤーの後ろ方向に向かって星を放出するパーティクル
/// </summary>
class StarParticle : public IParticle
{
public: // メンバ関数

	// 基底クラスのコンストラクタを使用
	using IParticle::IParticle;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime) override;



private: // メンバ変数

};

