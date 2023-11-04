#include "PlayerAnimComp.h"
#include "ModelComp.h"
#include "../../../Engine/DirectBase/Model/ModelManager.h"

void PlayerAnimComp::Init()
{
	// モデルコンポーネントの追加
	object_->AddComponent<ModelComp>();
	// モデルコンポーネントを取得しボーン追加
	auto* bodyBone = object_->GetComponent<ModelComp>()->AddBone("Body", ModelManager::GetInstance()->GetModel("PlayerBody"), bodyTransform_); // 体
	object_->GetComponent<ModelComp>()->AddBone("Eye", ModelManager::GetInstance()->GetModel("PlayerEye"), bodyBone, eyeTransform_); // 目
	object_->GetComponent<ModelComp>()->AddBone("Helmet", ModelManager::GetInstance()->GetModel("PlayerHelmet"), bodyBone, bodyTransform_); // ヘルメット
	object_->GetComponent<ModelComp>()->AddBone("Ling", ModelManager::GetInstance()->GetModel("PlayerLing"), LingTransform_); // 輪っか
	object_->GetComponent<ModelComp>()->AddBone("Arm_L", ModelManager::GetInstance()->GetModel("PlayerArm_L"), ArmTransform_L_); // 左腕
	object_->GetComponent<ModelComp>()->AddBone("Arm_R", ModelManager::GetInstance()->GetModel("PlayerArm_R"), ArmTransform_R_); // 右腕
	object_->GetComponent<ModelComp>()->AddBone("Foot_L", ModelManager::GetInstance()->GetModel("PlayerFoot_L"), FootTransform_L_); // 左足
	object_->GetComponent<ModelComp>()->AddBone("Foot_R", ModelManager::GetInstance()->GetModel("PlayerFoot_R"), FootTransform_R_); // 右足
}

void PlayerAnimComp::Update()
{
	// 自分自身のモデルコンポーネント
	auto *const modelComp = object_->GetComponent<ModelComp>();

	ImGui::Begin("playerAnim");
	if (ImGui::TreeNode("Base")) {
		ImGui::DragFloat3("scale", &transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Body")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Body")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Body")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Body")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Eye")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Eye")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Eye")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Eye")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Ling")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Ling")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Ling")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Ling")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Arm_L")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Arm_L")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Arm_L")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Arm_L")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Arm_R")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Arm_R")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Arm_R")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Arm_R")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Foot_L")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Foot_L")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Foot_L")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Foot_L")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Foot_R")) {
		ImGui::DragFloat3("scale", &modelComp->GetBone("Foot_R")->transform_->scale.x, 0.05f);
		ImGui::DragFloat3("rotate", &modelComp->GetBone("Foot_R")->transform_->rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &modelComp->GetBone("Foot_R")->transform_->translate.x, 0.5f);
		ImGui::TreePop();
	}
	ImGui::End();
}
