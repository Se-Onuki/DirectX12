#include "StarParticle.h"

void StarParticle::Init() {
	
}

void StarParticle::Update([[maybe_unused]] float deltaTime)
{
	transform_.rotate.y += 0.01f;
	transform_.rotate.y = Angle::Mod(transform_.rotate.y);



	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
