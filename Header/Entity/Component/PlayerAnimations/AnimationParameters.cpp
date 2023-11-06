#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	startIdle_.Initialize();
	endIdle_.Initialize();
}

void AnimationParameters::Update()
{
#ifdef _DEBUG
	ImGui::Begin("AnimManager");
	// アイドル状態のパラメーター
	if (ImGui::TreeNode("Idle")) {
		startIdle_.DisplayImGui("IdleStart");
		endIdle_.DisplayImGui("IdleEnd");
		ImGui::DragFloat("IdleAnimationTime", &idleAnimationTime_, 0.5f, 0.5f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();

	// アイドル関係
	const char* const idleName = "Idle";
	// グローバル変数クラスに一度アイテムを追加
	
	startIdle_.AddItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.AddItem(globalVariables_, "Idle", "IdleEnd");
	globalVariables_->SetValue(idleName, "Idle_AnimationTime", idleAnimationTime_);

#endif // _DEBUG
}

void AnimationParameters::AddItem()
{
	// アイドル関係
	const char* const idleName = "Idle";
	// グローバル変数クラスに一度アイテムを追加
	startIdle_.AddItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.AddItem(globalVariables_, "Idle", "IdleEnd");
	globalVariables_->AddValue(idleName, "Idle_AnimationTime", idleAnimationTime_);
}

void AnimationParameters::SetItem()
{
	startIdle_.SetItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.SetItem(globalVariables_, "Idle", "IdleEnd");
}

void AnimationParameters::ApplyItem()
{
	// アイドル関係
	const char* const idleName = "Idle";
	// グローバル変数クラスに一度アイテムを追加
	startIdle_.ApplyItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.ApplyItem(globalVariables_, "Idle", "IdleEnd");
	idleAnimationTime_ = globalVariables_->Get<float>(idleName, "Idle_AnimationTime");
}
