#include "BaseAnimation.h"

void BaseAnimation::Initialize(float transitionTime)
{
	// グローバル変数クラスのインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();
	
	// 遷移秒数を取得
	transitionTime_ = transitionTime;
	isTransitioning_ = false;
	// 遷移秒数が入力されていない、またはマイナスの場合は遷移を行わない
	if (transitionTime_ > 0) {
		isTransitioning_ = true;
		prevBone_ = GetPlayerBone(); // 現状のプレイヤー状態を取得しておく
	}
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

PlayerBone::Bone BaseAnimation::GetPlayerBone()
{
	// 一時保存用
	PlayerBone::Bone tempBone;
	// プレイヤーのモデルコンポーネント取得
	auto* const modelComp = entity_->GetComponent<ModelComp>();
	// 構造体に
	tempBone.body.scale = modelComp->GetBone("Body")->transform_->scale;
	tempBone.body.rotate = modelComp->GetBone("Body")->transform_->rotate;
	tempBone.body.translate = modelComp->GetBone("Body")->transform_->translate;
	tempBone.eye.scale = modelComp->GetBone("Eye")->transform_->scale;
	tempBone.eye.rotate = modelComp->GetBone("Eye")->transform_->rotate;
	tempBone.eye.translate = modelComp->GetBone("Eye")->transform_->translate;
	tempBone.ling.scale = modelComp->GetBone("Ling")->transform_->scale;
	tempBone.ling.rotate = modelComp->GetBone("Ling")->transform_->rotate;
	tempBone.ling.translate = modelComp->GetBone("Ling")->transform_->translate;
	tempBone.arm_L.scale = modelComp->GetBone("Arm_L")->transform_->scale;
	tempBone.arm_L.rotate = modelComp->GetBone("Arm_L")->transform_->rotate;
	tempBone.arm_L.translate = modelComp->GetBone("Arm_L")->transform_->translate;
	tempBone.arm_R.scale = modelComp->GetBone("Arm_R")->transform_->scale;
	tempBone.arm_R.rotate = modelComp->GetBone("Arm_R")->transform_->rotate;
	tempBone.arm_R.translate = modelComp->GetBone("Arm_R")->transform_->translate;
	tempBone.foot_L.scale = modelComp->GetBone("Foot_L")->transform_->scale;
	tempBone.foot_L.rotate = modelComp->GetBone("Foot_L")->transform_->rotate;
	tempBone.foot_L.translate = modelComp->GetBone("Foot_L")->transform_->translate;
	tempBone.foot_R.scale = modelComp->GetBone("Foot_R")->transform_->scale;
	tempBone.foot_R.rotate = modelComp->GetBone("Foot_R")->transform_->rotate;
	tempBone.foot_R.translate = modelComp->GetBone("Foot_R")->transform_->translate;

	// 結果を返す
	return tempBone;
}
