#include "PlayerBone.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"

void PlayerBone::Initialize()
{
	// ボーンの初期化
	bone_.Initilaize();
}

void PlayerBone::Initialize(Entity* player)
{
	// ボーンの初期化
	bone_.Initilaize();

	// エンティティを取得
	player_ = player;

	// プレイヤーのモデルコンポーネント取得
	auto* const modelComp = player_->GetComponent<ModelComp>();
	// 構造体に
	bone_.body.scale = modelComp->GetBone("Body")->transform_->scale;
	bone_.body.rotate = modelComp->GetBone("Body")->transform_->rotate;
	bone_.body.translate = modelComp->GetBone("Body")->transform_->translate;
	bone_.eye.scale = modelComp->GetBone("Eye")->transform_->scale;
	bone_.eye.rotate = modelComp->GetBone("Eye")->transform_->rotate;
	bone_.eye.translate = modelComp->GetBone("Eye")->transform_->translate;
	bone_.ling.scale = modelComp->GetBone("Ling")->transform_->scale;
	bone_.ling.rotate = modelComp->GetBone("Ling")->transform_->rotate;
	bone_.ling.translate = modelComp->GetBone("Ling")->transform_->translate;
	bone_.arm_L.scale = modelComp->GetBone("Arm_L")->transform_->scale;
	bone_.arm_L.rotate = modelComp->GetBone("Arm_L")->transform_->rotate;
	bone_.arm_L.translate = modelComp->GetBone("Arm_L")->transform_->translate;
	bone_.arm_R.scale = modelComp->GetBone("Arm_R")->transform_->scale;
	bone_.arm_R.rotate = modelComp->GetBone("Arm_R")->transform_->rotate;
	bone_.arm_R.translate = modelComp->GetBone("Arm_R")->transform_->translate;
	bone_.foot_L.scale = modelComp->GetBone("Foot_L")->transform_->scale;
	bone_.foot_L.rotate = modelComp->GetBone("Foot_L")->transform_->rotate;
	bone_.foot_L.translate = modelComp->GetBone("Foot_L")->transform_->translate;
	bone_.foot_R.scale = modelComp->GetBone("Foot_R")->transform_->scale;
	bone_.foot_R.rotate = modelComp->GetBone("Foot_R")->transform_->rotate;
	bone_.foot_R.translate = modelComp->GetBone("Foot_R")->transform_->translate;
}

void PlayerBone::SetToEntity()
{
	// プレイヤーのモデルコンポーネント取得
	auto* const modelComp = player_->GetComponent<ModelComp>();
	// 構造体に
	modelComp->GetBone("Body")->transform_->scale = bone_.body.scale;
	modelComp->GetBone("Body")->transform_->rotate = bone_.body.rotate;
	modelComp->GetBone("Body")->transform_->translate = bone_.body.translate;
	modelComp->GetBone("Eye")->transform_->scale = bone_.eye.scale;
	modelComp->GetBone("Eye")->transform_->rotate = bone_.eye.rotate;
	modelComp->GetBone("Eye")->transform_->translate = bone_.eye.translate;
	modelComp->GetBone("Ling")->transform_->scale = bone_.ling.scale;
	modelComp->GetBone("Ling")->transform_->rotate = bone_.ling.rotate;
	modelComp->GetBone("Ling")->transform_->translate = bone_.ling.translate;
	modelComp->GetBone("Arm_L")->transform_->scale = bone_.arm_L.scale;
	modelComp->GetBone("Arm_L")->transform_->rotate = bone_.arm_L.rotate;
	modelComp->GetBone("Arm_L")->transform_->translate = bone_.arm_L.translate;
	modelComp->GetBone("Arm_R")->transform_->scale = bone_.arm_R.scale;
	modelComp->GetBone("Arm_R")->transform_->rotate = bone_.arm_R.rotate;
	modelComp->GetBone("Arm_R")->transform_->translate = bone_.arm_R.translate;
	modelComp->GetBone("Foot_L")->transform_->scale = bone_.foot_L.scale;
	modelComp->GetBone("Foot_L")->transform_->rotate = bone_.foot_L.rotate;
	modelComp->GetBone("Foot_L")->transform_->translate = bone_.foot_L.translate;
	modelComp->GetBone("Foot_R")->transform_->scale = bone_.foot_R.scale;
	modelComp->GetBone("Foot_R")->transform_->rotate = bone_.foot_R.rotate;
	modelComp->GetBone("Foot_R")->transform_->translate = bone_.foot_R.translate;
}

void PlayerBone::AddItem(GlobalVariables* gv, std::string groupName)
{
	gv->AddValue(groupName, groupName + "Body_Scale", bone_.body.scale);
	gv->SetValue(groupName, groupName + "Body_Scale", bone_.body.scale);
	gv->AddValue(groupName, groupName + "Body_Rotate", bone_.body.rotate);
	gv->SetValue(groupName, groupName + "Body_Rotate", bone_.body.rotate);
	gv->AddValue(groupName, groupName + "Body_Translate", bone_.body.translate);
	gv->SetValue(groupName, groupName + "Body_Translate", bone_.body.translate);
	gv->AddValue(groupName, groupName + "Eye_Scale", bone_.eye.scale);
	gv->SetValue(groupName, groupName + "Eye_Scale", bone_.eye.scale);
	gv->AddValue(groupName, groupName + "Eye_Rotate", bone_.eye.rotate);
	gv->SetValue(groupName, groupName + "Eye_Rotate", bone_.eye.rotate);
	gv->AddValue(groupName, groupName + "Eye_Translate", bone_.eye.translate);
	gv->SetValue(groupName, groupName + "Eye_Translate", bone_.eye.translate);
	gv->AddValue(groupName, groupName + "Ling_Scale", bone_.ling.scale);
	gv->SetValue(groupName, groupName + "Ling_Scale", bone_.ling.scale);
	gv->AddValue(groupName, groupName + "Ling_Rotate", bone_.ling.rotate);
	gv->SetValue(groupName, groupName + "Ling_Rotate", bone_.ling.rotate);
	gv->AddValue(groupName, groupName + "Ling_Translate", bone_.ling.translate);
	gv->SetValue(groupName, groupName + "Ling_Translate", bone_.ling.translate);
	gv->AddValue(groupName, groupName + "Arm_L_Scale", bone_.arm_L.scale);
	gv->SetValue(groupName, groupName + "Arm_L_Scale", bone_.arm_L.scale);
	gv->AddValue(groupName, groupName + "Arm_L_Rotate", bone_.arm_L.rotate);
	gv->SetValue(groupName, groupName + "Arm_L_Rotate", bone_.arm_L.rotate);
	gv->AddValue(groupName, groupName + "Arm_L_Translate", bone_.arm_L.translate);
	gv->SetValue(groupName, groupName + "Arm_L_Translate", bone_.arm_L.translate);
	gv->AddValue(groupName, groupName + "Arm_R_Scale", bone_.arm_R.scale);
	gv->SetValue(groupName, groupName + "Arm_R_Scale", bone_.arm_R.scale);
	gv->AddValue(groupName, groupName + "Arm_R_Rotate", bone_.arm_R.rotate);
	gv->SetValue(groupName, groupName + "Arm_R_Rotate", bone_.arm_R.rotate);
	gv->AddValue(groupName, groupName + "Arm_R_Translate", bone_.arm_R.translate);
	gv->SetValue(groupName, groupName + "Arm_R_Translate", bone_.arm_R.translate);
	gv->AddValue(groupName, groupName + "Foot_L_Scale", bone_.foot_L.scale);
	gv->SetValue(groupName, groupName + "Foot_L_Scale", bone_.foot_L.scale);
	gv->AddValue(groupName, groupName + "Foot_L_Rotate", bone_.foot_L.rotate);
	gv->SetValue(groupName, groupName + "Foot_L_Rotate", bone_.foot_L.rotate);
	gv->AddValue(groupName, groupName + "Foot_L_Translate", bone_.foot_L.translate);
	gv->SetValue(groupName, groupName + "Foot_L_Translate", bone_.foot_L.translate);
	gv->AddValue(groupName, groupName + "Foot_R_Scale", bone_.foot_R.scale);
	gv->SetValue(groupName, groupName + "Foot_R_Scale", bone_.foot_R.scale);
	gv->AddValue(groupName, groupName + "Foot_R_Rotate", bone_.foot_R.rotate);
	gv->SetValue(groupName, groupName + "Foot_R_Rotate", bone_.foot_R.rotate);
	gv->AddValue(groupName, groupName + "Foot_R_Translate", bone_.foot_R.translate);
	gv->SetValue(groupName, groupName + "Foot_R_Translate", bone_.foot_R.translate);

}

void PlayerBone::ApplyItem(GlobalVariables* gv, std::string groupName)
{
	bone_.body.scale = gv->Get<Vector3>(groupName, groupName + "Body_Scale");
	bone_.body.rotate = gv->Get<Vector3>(groupName, groupName + "Body_Rotate");
	bone_.body.translate = gv->Get<Vector3>(groupName, groupName + "Body_Translate");
	bone_.eye.scale = gv->Get<Vector3>(groupName, groupName + "Eye_Scale");
	bone_.eye.rotate = gv->Get<Vector3>(groupName, groupName + "Eye_Rotate");
	bone_.eye.translate = gv->Get<Vector3>(groupName, groupName + "Eye_Translate");
	bone_.ling.scale = gv->Get<Vector3>(groupName, groupName + "Ling_Scale");
	bone_.ling.rotate = gv->Get<Vector3>(groupName, groupName + "Ling_Rotate");
	bone_.ling.translate = gv->Get<Vector3>(groupName, groupName + "Ling_Translate");
	bone_.arm_L.scale = gv->Get<Vector3>(groupName, groupName + "Arm_L_Scale");
	bone_.arm_L.rotate = gv->Get<Vector3>(groupName, groupName + "Arm_L_Rotate");
	bone_.arm_L.translate = gv->Get<Vector3>(groupName, groupName + "Arm_L_Translate");
	bone_.arm_R.scale = gv->Get<Vector3>(groupName, groupName + "Arm_R_Scale");
	bone_.arm_R.rotate = gv->Get<Vector3>(groupName, groupName + "Arm_R_Rotate");
	bone_.arm_R.translate = gv->Get<Vector3>(groupName, groupName + "Arm_R_Translate");
	bone_.foot_L.scale = gv->Get<Vector3>(groupName, groupName + "Foot_L_Scale");
	bone_.foot_L.rotate = gv->Get<Vector3>(groupName, groupName + "Foot_L_Rotate");
	bone_.foot_L.translate = gv->Get<Vector3>(groupName, groupName + "Foot_L_Translate");
	bone_.foot_R.scale = gv->Get<Vector3>(groupName, groupName + "Foot_R_Scale");
	bone_.foot_R.rotate = gv->Get<Vector3>(groupName, groupName + "Foot_R_Rotate");
	bone_.foot_R.translate = gv->Get<Vector3>(groupName, groupName + "Foot_R_Translate");

}

void PlayerBone::DisplayImGui(std::string groupName)
{
	ImGui::Begin("AnimManager");
	if (ImGui::TreeNode(groupName.c_str())) {
		if (ImGui::TreeNode("Body")) {
			ImGui::DragFloat3("Body_Scale", &bone_.body.scale.x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Body_Rotate", &bone_.body.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Body_Translate", &bone_.body.translate.x, 0.05f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Eye")) {
			ImGui::DragFloat3("Eye_Scale", &bone_.eye.scale.x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Eye_Rotate", &bone_.eye.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Eye_Translate", &bone_.eye.translate.x, 0.05f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Ling")) {
			ImGui::DragFloat3("Ling_Scale", &bone_.ling.scale.x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Ling_Rotate", &bone_.ling.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Ling_Translate", &bone_.ling.translate.x, 0.05f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Arm_L")) {
			ImGui::DragFloat3("Arm_L_Scale", &bone_.arm_L.scale.x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Arm_L_Rotate", &bone_.arm_L.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Arm_L_Translate", &bone_.arm_L.translate.x, 0.05f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Arm_R")) {
			ImGui::DragFloat3("Arm_R_Scale", &bone_.arm_R.scale.x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Arm_R_Rotate", &bone_.arm_R.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Arm_R_Translate", &bone_.arm_R.translate.x, 0.05f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Foot_L")) {
			ImGui::DragFloat3("Foot_L_Scale", &bone_.foot_L.scale.x, 0.5f, -100.0f, 100.0f);
			ImGui::DragFloat3("Foot_L_Rotate", &bone_.foot_L.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Foot_L_Translate", &bone_.foot_L.translate.x, 0.5f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Foot_R")) {
			ImGui::DragFloat3("Foot_R_Scale", &bone_.foot_R.scale.x, 0.5f, -100.0f, 100.0f);
			ImGui::DragFloat3("Foot_R_Rotate", &bone_.foot_R.rotate.x, 0.01f, (float)-std::numbers::pi * 2.0f, (float)std::numbers::pi * 2.0f);
			ImGui::DragFloat3("Foot_R_Translate", &bone_.foot_R.translate.x, 0.5f, -1000.0f, 1000.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();
}
