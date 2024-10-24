#include "SimpleParticle.h"
#include "../../Engine/Utils/Math/Random.h"

void SimpleParticle::Init()
{
}

void SimpleParticle::Update([[maybe_unused]] float deltaTime)
{
	if (!timer_.IsFinish()) {
		acceleration_ -= velocity_ * 0.1f; // 空気抵抗
		velocity_ += acceleration_;	// 速度の代入
		acceleration_ = {};	// 加速度のリセット
		transform_.translate += velocity_ * deltaTime;
		color_.a = (1.f - timer_.GetProgress());
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
