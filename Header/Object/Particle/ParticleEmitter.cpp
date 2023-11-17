#include "ParticleEmitter.h"
#include "StarParticle.h"

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

	// 初期設定
	emitIntervalTimer_.Start(1.0f); // 発生間隔タイマーの初期化
	SetParticleType<StarParticle>(); // 型の設定
}

void IParticleEmitter::Update([[maybe_unused]]float deltaTime)
{
	// 終了している粒子があった場合それをリストから追い出す
	particles_.remove_if([](IParticle* particle) {
		if (not particle->isAlive_) {
			return true;
		}
		return false;
	});

	// 粒子の数が最大数に達していなかった場合
	if (particles_.size() <= maxParticleCount_) {
		if (emitIntervalTimer_.IsFinish()) {
			CreateParticle(*model_);
		}
		else {
			
		}
	}
}

void IParticleEmitter::DisplayImGui()
{

}

IParticle* IParticleEmitter::CreateParticle(const Model& model)
{
	// 選択されたパーティクルを生成する
	IParticle* particle = particleManager_->AddParticle(&model, type_());
	// 生成したパーティクルにパラメーターを設定
	particle->transform_ = emitTransform_;			 // 発生座標
	particle->velocity_ = particleVelocity_;		 // 速度設定
	particle->acceleration_ = particleAcceleration_; // 加速度設定
	particle->randomNumber_ = emitBlur_;			 // 乱数設定
	particle->timer_.Start(particleAliveTme_);		 // 粒子タイマー
	particle->Init();								 // 再度初期化

	// 生成したパーティクルを返す
	return particle;
}
