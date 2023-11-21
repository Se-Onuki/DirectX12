#include "TestParticle.h"
#include "../../../Utils/Math/Random.h"

void TestParticle::Init()
{
	transform_.translate.x = Random::GetRandom<float>(-10.0f, 10.0f);
}

void TestParticle::Update([[maybe_unused]] float deltaTime)
{
	if (!timer_.IsFinish()) {
		
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
