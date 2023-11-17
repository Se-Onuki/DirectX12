#pragma once
#include "ParticleManager.h"
#include <typeinfo>

/// <summary>
/// パーティクル生成クラス
/// </summary>
class ParticleEmitter
{


public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	virtual void Initialize();

	template<IsIParticle SelectParticle>
	inline void CreateParticle(const Model& model) {
		particleManager_->AddParticle(model, std::make_unique<SelectParticle>());
	}

protected: // 継承先メンバ変数メンバ変数

	// パーティクルマネージャ
	ParticleManager* particleManager_;

	// 発生中心座標


	// エミッタの継続時間


	// パーティクル自体のブレ

};
