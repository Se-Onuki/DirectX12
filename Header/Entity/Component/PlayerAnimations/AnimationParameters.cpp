#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	startIdle_.Initialize();
	endIdle_.Initialize();
	startMove_.Initialize();
	endMove_.Initialize();
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

	if (ImGui::TreeNode("Move")) {
		startMove_.DisplayImGui("MoveStart");
		endMove_.DisplayImGui("MoveEnd");
		ImGui::DragFloat("MoveAnimationTime", &moveAnimationTime_, 0.5f, 0.5f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();

	// グローバル変数クラスに一度アイテムを追加
	startIdle_.AddItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.AddItem(globalVariables_, "Idle", "IdleEnd");
	globalVariables_->SetValue("Idle", "Idle_AnimationTime", idleAnimationTime_);
	startMove_.AddItem(globalVariables_, "Move", "MoveStart");
	endMove_.AddItem(globalVariables_, "Move", "MoveEnd");
	globalVariables_->SetValue("Move", "Move_AnimationTime", moveAnimationTime_);

#endif // _DEBUG
}

void AnimationParameters::AddItem()
{
	// グローバル変数クラスに一度アイテムを追加
	startIdle_.AddItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.AddItem(globalVariables_, "Idle", "IdleEnd");
	globalVariables_->AddValue("Idle", "Idle_AnimationTime", idleAnimationTime_);
	startMove_.AddItem(globalVariables_, "Move", "MoveStart");
	endMove_.AddItem(globalVariables_, "Move", "MoveEnd");
	globalVariables_->AddValue("Move", "Move_AnimationTime", moveAnimationTime_);
}

void AnimationParameters::SetItem()
{
	startIdle_.SetItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.SetItem(globalVariables_, "Idle", "IdleEnd");
	startMove_.SetItem(globalVariables_, "Move", "MoveStart");
	endMove_.SetItem(globalVariables_, "Move", "MoveEnd");
}

void AnimationParameters::ApplyItem()
{
	// グローバル変数クラスに一度アイテムを追加
	startIdle_.ApplyItem(globalVariables_, "Idle", "IdleStart");
	endIdle_.ApplyItem(globalVariables_, "Idle", "IdleEnd");
	idleAnimationTime_ = globalVariables_->Get<float>("Idle", "Idle_AnimationTime");
	startMove_.ApplyItem(globalVariables_, "Move", "MoveStart");
	endMove_.ApplyItem(globalVariables_, "Move", "MoveEnd");
	moveAnimationTime_ = globalVariables_->Get<float>("Move", "Move_AnimationTime");

}
