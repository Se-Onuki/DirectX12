#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	// 各種キーの初期化
	idleAnimKeys_.Initialize("Idle");
	moveAnimKeys_.Initialize("Move");
	startJumpAnimKeys_.Initialize("StartJump");
	hoveringAnimKeys_.Initialize("Hovering");
	landAnimKeys_.Initialize("Land");
	startRotateAnimKeys_.Initialize("RotateStart");
	rotatingAnimKeys_.Initialize("Rotating");
	rotateEndAnimationKeys_.Initialize("RotateEnd");
	clearAnimationKeys_.Initialize("Clear");
	clearBleakAnimationKeys_.Initialize("ClearBleak");
	titleAnimationKeys_.Initialize("Title");
}

void AnimationParameters::Update()
{
#ifdef _DEBUG
	idleAnimKeys_.ShowImGUi();
	moveAnimKeys_.ShowImGUi();
	startJumpAnimKeys_.ShowImGUi();
	hoveringAnimKeys_.ShowImGUi();
	landAnimKeys_.ShowImGUi();
	startRotateAnimKeys_.ShowImGUi();
	rotatingAnimKeys_.ShowImGUi();
	rotateEndAnimationKeys_.ShowImGUi();
	clearAnimationKeys_.ShowImGUi();
	clearBleakAnimationKeys_.ShowImGUi();
	titleAnimationKeys_.ShowImGUi();
#endif // _DEBUG
}
