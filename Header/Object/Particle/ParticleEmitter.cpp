#include "ParticleEmitter.h"
#include "StarParticle.h"

void ParticleEmitter::Init(float aliveTime)
{
	// インスタンス取得
	particleManager_ = ParticleManager::GetInstance(); // パーティクルマネージャー
	gv_ = GlobalVariables::GetInstance(); // 調整項目クラス
	// サンプルモデルをセット
	model_ = ModelManager::GetInstance()->GetModel("Box");

	// 粒子リストのクリア
	particles_.clear();

	// エミッタの継続時間をリセット
	emitAliveTimer_.Start(aliveTime);

	// 初期設定
	emitIntervalTimer_.Start(0.0f); // 発生間隔タイマーの初期化
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
	// ボタンを押すとパラメーターをセット
	if (ImGui::Button("SetItem")) {
		AddItem();
		SetItem();
	}
	if (ImGui::Button("ApplyItem")) {
		ApplyItem();
	}

	if (ImGui::Button("SaveParameters")) {
		gv_->SaveFile(typeName_);
	}

	ImGui::Text("Parameters");
	ImGui::InputInt("ParticleCount", &maxParticleCount_, 1, 300);
	ImGui::DragFloat("EmitInterval", &emitInterval_,0.01f, 0.01f, 25.0f);
	ImGui::DragFloat3("Velocity", &particleVelocity_.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Acceleration", &particleAcceleration_.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat("EmitBlur", &emitBlur_, 0.01f, 0.0f, 50.0f);
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
	particle->Init();								 // 再度初期化
	// 粒子配列に追加
	particles_.push_back(particle);

	// 生成したパーティクルを返す
	return particle;
}

void ParticleEmitter::AddItem()
{
	// 調整項目クラスに値を追加
	gv_->AddValue(typeName_, "MaxParticleCount", maxParticleCount_);		 // 粒子最大数
	gv_->AddValue(typeName_, "EmitInterval", emitInterval_);				 // 粒子生成間隔
	gv_->AddValue(typeName_, "ParticleVelocity", particleVelocity_);		 // 粒子速度ベクトル
	gv_->AddValue(typeName_, "ParticleAcceleration", particleAcceleration_); // 粒子加速度
	gv_->AddValue(typeName_, "EmitBlur", emitBlur_);						 // 粒子ブレ
}

void ParticleEmitter::SetItem()
{
	// 調整項目クラスに値をセット
	gv_->SetValue(typeName_, "MaxParticleCount", maxParticleCount_);		 // 粒子最大数
	gv_->SetValue(typeName_, "EmitInterval", emitInterval_);				 // 粒子生成間隔
	gv_->SetValue(typeName_, "ParticleVelocity", particleVelocity_);		 // 粒子速度ベクトル
	gv_->SetValue(typeName_, "ParticleAcceleration", particleAcceleration_); // 粒子加速度
	gv_->SetValue(typeName_, "EmitBlur", emitBlur_);						 // 粒子ブレ
}

void ParticleEmitter::ApplyItem()
{
	maxParticleCount_ = gv_->Get<int>(typeName_, "MaxParticleCount");
	emitInterval_ = gv_->Get<float>(typeName_, "EmitInterval");
	particleVelocity_ = gv_->Get<Vector3>(typeName_, "ParticleVelocity");
	particleAcceleration_ = gv_->Get<Vector3>(typeName_, "ParticleAcceleration");
	emitBlur_ = gv_->Get<float>(typeName_, "EmitBlur");
}
