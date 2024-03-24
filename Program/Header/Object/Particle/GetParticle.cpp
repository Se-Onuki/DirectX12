#include "GetParticle.h"
// 静的なメンバ変数の実体を宣言
float GetParticle::minSize_ = 0.5f;
float GetParticle::maxSize_ = 0.5f;
float GetParticle::dispalyParticleTime_ = 0.05f;

void GetParticle::AddItem()
{
}

void GetParticle::SetItem()
{
}

void GetParticle::DisplayImGui()
{
}

void GetParticle::Init()
{
	// 大きさの設定
	transform_.scale.x = Random::GetRandom<float>(minSize_, maxSize_);
	transform_.scale = { transform_.scale.x , transform_.scale.x , transform_.scale.x };
	// 大きさ取得
	startScale_ = transform_.scale;

	// 放出角度をXYZそれぞれ求める
	transform_.rotate = { Random::GetRandom<float>(-(float)std::numbers::pi, (float)std::numbers::pi), Random::GetRandom<float>(-(float)std::numbers::pi, (float)std::numbers::pi), Random::GetRandom<float>(-(float)std::numbers::pi, (float)std::numbers::pi) };

	// 座標の設定
	float blur = Random::GetRandom<float>(-randomNumber_, randomNumber_);
	transform_.translate.x += blur;
	blur = Random::GetRandom<float>(-randomNumber_, randomNumber_);
	transform_.translate.y += blur;
	blur = Random::GetRandom<float>(-randomNumber_, randomNumber_);
	transform_.translate.z += blur;

	// 粒子表示までのタイマー設定
	diplayParticleTimer_.Start(dispalyParticleTime_);
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// エミッタの角度から放出方向を求める
	const Matrix4x4& mat = transform_.rotate.MakeRotateMatrix(); // 回転行列の生成
	move_ = TransformNormal(velocity_, mat);
}

void GetParticle::Update(float deltaTime)
{
	if (!timer_.IsFinish()) {
		transform_.translate += move_;

		// 線形補間によってスケールを動かす
		transform_.scale = SoLib::Lerp<Vector3>(startScale_, { 0.0f, 0.0f,0.0f }, SoLib::easeLinear(timer_.GetProgress()));
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
	diplayParticleTimer_.Update(deltaTime);
}
