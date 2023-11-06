#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	idleAnimKeys_.Initialize("Idle");
	MoveAnimKeys_.Initialize("Move");
}

void AnimationParameters::Update()
{
#ifdef _DEBUG
	idleAnimKeys_.ShowImGUi();
	MoveAnimKeys_.ShowImGUi();
#endif // _DEBUG
}
