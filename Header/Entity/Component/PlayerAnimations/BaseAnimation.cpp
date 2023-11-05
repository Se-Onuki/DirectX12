#include "BaseAnimation.h"

void BaseAnimation::Initialize()
{
	// グローバル変数クラスのインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();
}

void BaseAnimation::Update(float deltaTime)
{
	// 基底クラスでは処理を記述しない
	// 以下は警告回避用の記述
	deltaTime;
}
