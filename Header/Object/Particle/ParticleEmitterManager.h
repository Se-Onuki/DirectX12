#pragma once
#include <list>
#include <map>
#include <typeindex>
#include "ParticleEmitter.h"

/// <summary>
/// パーティクルエミッターの管理マネージャー
/// </summary>
class ParticleEmitterManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	ParticleEmitterManager() = default;
	~ParticleEmitterManager() = default;
	ParticleEmitterManager(const ParticleEmitterManager&) = delete;
	const ParticleEmitterManager& operator=(const ParticleEmitterManager&) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleEmitterManager* GetInstance() {
		static ParticleEmitterManager instance;
		return &instance;
	}

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 引数で指定したパーティクルエミッタを生成する関数
	/// </summary>
	/// <typeparam name="SelectParticleEmitter">生成するパーティクルの型</typeparam>
	/// <param name="model">パーティクルに使用するモデル</param>
	/// <param name="aliveTime">パーティクルの継続時間</param>
	template<IsIParticleEmitter SelectParticleEmitter>
	inline void CreateEmitter(const Model* model, float aliveTime) {
		// インスタンスの生成
		std::unique_ptr<IParticleEmitter> emitter = std::make_unique<SelectParticleEmitter>();
		// 生成したインスタンスの初期化
		emitter->Init(model, aliveTime);
		// 初期化したインスタンスを配列に追加
		emitters_.push_back(emitter);
	}

private: // メンバ変数

	// パーティクルエミッター達
	std::list<std::unique_ptr<IParticleEmitter>> emitters_;

};