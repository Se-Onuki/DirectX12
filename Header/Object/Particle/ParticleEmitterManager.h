#pragma once
#include <map>
#include <string>
#include <typeindex>
#include "ParticleEmitter.h"
#include "ParticleList.h"

/// <summary>
/// パーティクルエミッターの管理マネージャー
/// </summary>
class ParticleEmitterManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	ParticleEmitterManager() = default;
	~ParticleEmitterManager() = default;
	ParticleEmitterManager(const ParticleEmitterManager &) = delete;
	const ParticleEmitterManager &operator=(const ParticleEmitterManager &) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleEmitterManager *GetInstance() {
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
	/// <typeparam name="SelectParticle">生成するパーティクルの型</typeparam>
	/// <param name="model">パーティクルに使用するモデル</param>
	/// <param name="aliveTime">パーティクルの継続時間</param>
	template<IsIParticle SelectParticle>
	inline void CreateEmitter(const Model *model, float aliveTime) {
		// インスタンスの生成
		std::unique_ptr<ParticleEmitter> emitter = std::make_unique<ParticleEmitter>();
		// 生成したインスタンスの初期化
		emitter->Init(model, aliveTime);
		// 型タイプを設定
		emitter->SetParticleType<SelectParticle>();
		// 初期化したインスタンスを配列に追加
		emitters_.push_back(std::move(emitter));
	}

#ifdef _DEBUG
	/// <summary>
	/// パーティクルの型リストに追加する関数
	/// </summary>
	/// <typeparam name="SelectParticleEmitter">追加する型</typeparam>
	template<IsIParticle SelectParticle>
	void AddParticleMold() {
		moldMap_.insert({
			typeid(SelectParticle),
			std::function<std::unique_ptr<IParticle>(const Vector3 &)>([](const Vector3 &position)
				{
					return std::make_unique<SelectParticle>(position);
				})
			});
	}
#endif // _DEBUG

private: // メンバ変数

	// パーティクルエミッター達
	std::list<std::unique_ptr<ParticleEmitter>> emitters_;

#ifdef _DEBUG // ImGui用変数

	// パーティクル型マップ
	std::map <std::type_index, std::function<std::unique_ptr<IParticle>(const Vector3 &)>> moldMap_;

	//　変数
	const Vector3 position = { 0.0f, 0.0f, 0.0f };

#endif // _DEBUG

};