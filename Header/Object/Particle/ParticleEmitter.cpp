#include "ParticleEmitter.h"

void IParticleEmitter::Init(Model* model, float aliveTime)
{
	// インスタンス取得
	particleManager_ = ParticleManager::GetInstance();
	// モデルをセット
	model_ = model;

	// 粒子リストのクリア
	particles_.clear();

	// エミッタの継続時間をリセット
	emitAliveTimer_.Start(aliveTime);
}
