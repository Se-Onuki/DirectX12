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

PlayerBone::Bone BaseAnimation::GetTargetBone(std::string groupName, std::string boneName)
{
	// 一時保存用
	PlayerBone::Bone tempBone;
	// グローバル変数から値を取得
	tempBone.body.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Body_Scale");
	tempBone.body.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Body_Rotate");
	tempBone.body.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Body_Translate");
	tempBone.eye.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Eye_Scale");
	tempBone.eye.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Eye_Rotate");
	tempBone.eye.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Eye_Translate");
	tempBone.ling.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Ling_Scale");
	tempBone.ling.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Ling_Rotate");
	tempBone.ling.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Ling_Translate");
	tempBone.arm_L.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_L_Scale");
	tempBone.arm_L.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_L_Rotate");
	tempBone.arm_L.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_L_Translate");
	tempBone.arm_R.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_R_Scale");
	tempBone.arm_R.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_R_Rotate");
	tempBone.arm_R.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_R_Translate");
	tempBone.foot_L.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_L_Scale");
	tempBone.foot_L.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_L_Rotate");
	tempBone.foot_L.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_L_Translate");
	tempBone.foot_R.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_R_Scale");
	tempBone.foot_R.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_R_Rotate");
	tempBone.foot_R.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_R_Translate");

	// 結果を返す
	return tempBone;
}
