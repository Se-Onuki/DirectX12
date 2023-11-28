#include "TestParticle.h"
#include "../../../Utils/Math/Random.h"

void TestParticle::Init()
{
}

void TestParticle::Update([[maybe_unused]] float deltaTime)
{
	if (!timer_.IsFinish()) {
		transform_.translate += velocity_ * deltaTime;
		float reverseProgress = 1.f - timer_.GetProgress();
		transform_.scale = Vector3::one * reverseProgress;

		color_ = SoLib::Lerp(startColor_, endColor_, timer_.GetProgress());

	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
