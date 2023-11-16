#include "StarParticle.h"

StarParticle::StarParticle(const Vector3& translate, const Vector3& direction)
{
	// 引数の値をメンバ変数に代入
	transform_.translate = translate;
	emissionDirection_ = &direction;

}

void StarParticle::Init()
{
	
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
