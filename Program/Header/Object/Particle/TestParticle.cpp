#include "TestParticle.h"
#include "../../../Utils/Math/Random.h"

void TestParticle::Init()
{
}

void TestParticle::Update([[maybe_unused]] float deltaTime)
{
	if (!timer_.IsFinish()) {
		transform_.translate += velocity_ * deltaTime;
		color_.a = (1.f - timer_.GetProgress());
		transform_.scale = SoLib::Lerp(startScale_, endScale_, timer_.GetProgress());
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
