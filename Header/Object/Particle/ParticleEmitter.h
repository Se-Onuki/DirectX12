#pragma once
#include "ParticleManager.h"

/// <summary>
/// パーティクル生成クラス
/// </summary>
class ParticleEmitter
{
public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize();

	template<IsIParticle SelectParticle>
	void CreateParticle(const Model& model) {
		particleManager_->AddParticle(model, std::make_unique<SelectParticle>());
	}

private: // メンバ変数

	// パーティクルマネージャ
	ParticleManager* particleManager_;

};
