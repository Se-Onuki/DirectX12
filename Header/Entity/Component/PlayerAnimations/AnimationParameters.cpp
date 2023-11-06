#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	idleAnimKeys_.Initialize("Idle");
}

void AnimationParameters::Update()
{
#ifdef _DEBUG
	idleAnimKeys_.ShowImGUi();
#endif // _DEBUG
}

void AnimationParameters::AddItem()
{
	
}

void AnimationParameters::SetItem()
{
	
}

void AnimationParameters::ApplyItem()
{
	

}
