#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	idleAnimKeys_.Initialize("Idle");
	moveAnimKeys_.Initialize("Move");
	startJumpAnimKeys_.Initialize("StartJump");
	hoveringAnimKeys_.Initialize("Hovering");
	landAnimKeys_.Initialize("Land");
}

void AnimationParameters::Update()
{
#ifdef _DEBUG
	idleAnimKeys_.ShowImGUi();
	moveAnimKeys_.ShowImGUi();
	startJumpAnimKeys_.ShowImGUi();
	hoveringAnimKeys_.ShowImGUi();
	landAnimKeys_.ShowImGUi();
#endif // _DEBUG
}
