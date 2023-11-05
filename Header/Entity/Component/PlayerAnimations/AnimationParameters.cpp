#include "AnimationParameters.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void AnimationParameters::Initialize()
{
	// グローバル変数クラスにインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();
}

void AnimationParameters::Update()
{
#ifdef _DEBUG
	ImGui::Begin("AnimManager");
	// アイドル状態のパラメーター
	if (ImGui::TreeNode("Idle")) {
		ImGui::SliderFloat3("armSwingStartAngle", &armSwingStartAngle_.x, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
		ImGui::SliderFloat3("armSwingEndAngle", &armSwingEndAngle_.x, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
		ImGui::SliderFloat3("bodySwingStartAngle", &bodySwingStartAngle_.x, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
		ImGui::SliderFloat3("bodySwingEndAngle", &bodySwingEndAngle_.x, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
		ImGui::SliderFloat("IdleAnimationTime", &idleAnimationTime_, 0.5f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();

	// アイドル関係
	const char* const idleName = "Idle";
	// グローバル変数クラスに一度アイテムを追加
	globalVariables_->SetValue(idleName, "Idle_ArmSwingStartAngle", armSwingStartAngle_);
	globalVariables_->SetValue(idleName, "Idle_ArmSwingEndAngle", armSwingEndAngle_);
	globalVariables_->SetValue(idleName, "Idle_BodySwingStartAngle", bodySwingStartAngle_);
	globalVariables_->SetValue(idleName, "Idle_BodySwingEndAngle", bodySwingEndAngle_);
	globalVariables_->SetValue(idleName, "Idle_AnimationTime", idleAnimationTime_);

#endif // _DEBUG
}

void AnimationParameters::AddItem()
{
	// アイドル関係
	const char* const idleName = "Idle";
	// グローバル変数クラスに一度アイテムを追加
	globalVariables_->AddValue(idleName, "Idle_ArmSwingStartAngle", armSwingStartAngle_);
	globalVariables_->AddValue(idleName, "Idle_ArmSwingEndAngle", armSwingEndAngle_);
	globalVariables_->AddValue(idleName, "Idle_BodySwingStartAngle", bodySwingStartAngle_);
	globalVariables_->AddValue(idleName, "Idle_BodySwingEndAngle", bodySwingEndAngle_);
	globalVariables_->AddValue(idleName, "Idle_AnimationTime", idleAnimationTime_);
}

void AnimationParameters::ApplyItem()
{
	// アイドル関係
	const char* const idleName = "Idle";
	// グローバル変数クラスに一度アイテムを追加
	armSwingStartAngle_ = globalVariables_->Get<Vector3>(idleName, "Idle_ArmSwingStartAngle");
	armSwingEndAngle_ = globalVariables_->Get<Vector3>(idleName, "Idle_ArmSwingEndAngle");
	bodySwingStartAngle_ = globalVariables_->Get<Vector3>(idleName, "Idle_BodySwingStartAngle");
	bodySwingEndAngle_ = globalVariables_->Get<Vector3>(idleName, "Idle_BodySwingEndAngle");
	idleAnimationTime_ = globalVariables_->Get<float>(idleName, "Idle_AnimationTime");
}
