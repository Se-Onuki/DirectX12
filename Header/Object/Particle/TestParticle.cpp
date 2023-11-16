#include "TestParticle.h"
#include "../../../Utils/Math/Random.h"

void TestParticle::Init()
{
	color_ = Vector4{ 1.f,1.f,1.f,1.f };
	transform_.translate.x = Random::GetRandom<float>(-10.0f, 10.0f);
}

void TestParticle::Update([[maybe_unused]] float deltaTime)
{
	transform_.CalcMatrix();
}
