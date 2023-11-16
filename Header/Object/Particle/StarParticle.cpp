#include "StarParticle.h"

StarParticle::StarParticle(const Vector3& emissionDirection)
{
	// 引数の値をメンバ変数に代入
	emissionDirection_ = &emissionDirection;
}

void StarParticle::Init()
{
	color_ = { 1.f,1.f,1.f,1.f };
	transform_.translate.y = 3.0f;
}

void StarParticle::Update([[maybe_unused]] float deltaTime)
{
	transform_.rotate.y += 0.01f;
	transform_.rotate.y = Angle::Mod(transform_.rotate.y);

	transform_.CalcMatrix();
}
