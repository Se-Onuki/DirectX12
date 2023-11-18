#include "ParticleEmitter.h"
#include "StarParticle.h"

void ParticleEmitter::Init(float aliveTime)
{

	// インスタンス取得
	particleManager_ = ParticleManager::GetInstance(); // パーティクルマネージャー
	globalVariables_ = GlobalVariables::GetInstance(); // 調整項目クラス
	// サンプルモデルをセット
	model_ = ModelManager::GetInstance()->GetModel("Box");

	// 粒子リストのクリア
	particles_.clear();

	// エミッタの継続時間をリセット
	emitAliveTimer_.Start(aliveTime);

	// 初期設定
	emitIntervalTimer_.Start(0.1f); // 発生間隔タイマーの初期化
	SetParticleType<StarParticle>(); // 型の設定
}

void ParticleEmitter::Update([[maybe_unused]]float deltaTime)
{
	// 終了している粒子があった場合それをリストから追い出す
	particles_.remove_if([](IParticle* particle) {
		if (not particle->isAlive_) {
			return true;
		}
		return false;
	});

	// 生存時間中なら
	if (!emitAliveTimer_.IsFinish()) {
		// 粒子の数が最大数に達していなかった場合
		if (particles_.size() <= maxParticleCount_) {
			if (emitIntervalTimer_.IsFinish()) {
				CreateParticle(*model_);				 // パーティクル生成
				emitIntervalTimer_.Start(emitInterval_); // タイマーリセット
			}
		}
	}
	else {
		// パーティクル粒子数が0になったら終了
		if (particles_.size() == 0) {
			isEnd_ = true;
		}
	}
	

	// タイマー更新
	emitAliveTimer_.Update(deltaTime);
	emitIntervalTimer_.Update(deltaTime);
}

void ParticleEmitter::DisplayImGui()
{

}

IParticle* ParticleEmitter::CreateParticle(const Model& model)
{
	// 選択されたパーティクルを生成する
	IParticle* particle = particleManager_->AddParticle(&model, type_({0.0f, 0.0f, 0.0f}));
	// 生成したパーティクルにパラメーターを設定
	particle->transform_ = emitTransform_;			 // 発生座標
	particle->velocity_ = particleVelocity_;		 // 速度設定
	particle->acceleration_ = particleAcceleration_; // 加速度設定
	particle->randomNumber_ = emitBlur_;			 // 乱数設定
	particle->timer_.Start(particleAliveTme_);		 // 粒子タイマー
	particle->Init();								 // 再度初期化
	// 粒子配列に追加
	particles_.push_back(particle);

	// 生成したパーティクルを返す
	return particle;
}
