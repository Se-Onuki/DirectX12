#include "StarParticle.h"

void StarParticle::Init() {
	timer_.Start(1.0f);
}

void StarParticle::Update([[maybe_unused]] float deltaTime)
{

	if (!timer_.IsFinish()) {
		transform_.rotate.y += 0.01f;
		transform_.rotate.y = Angle::Mod(transform_.rotate.y);
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
