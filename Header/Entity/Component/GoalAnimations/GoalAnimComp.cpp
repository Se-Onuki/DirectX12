#include "GoalAnimComp.h"
#include "../ModelComp.h"
#include "../PlayerAnimComp.h"
#include "../../../../Engine/DirectBase/Model/ModelManager.h"

void GoalAnimComp::Init()
{
	// モデルコンポーネントの追加
	object_->AddComponent<ModelComp>();
	// ゴール
	object_->GetComponent<ModelComp>()->AddBone("Base", ModelManager::GetInstance()->GetModel("Flag"));

	// カメラマネージャーの取得
	cameraManager_ = CameraManager::GetInstance();
	// ゴール時のカメラを生成
	goalCamera_ = cameraManager_->AddCamera("goalCamera");

}

void GoalAnimComp::Update()
{
	
	

#ifdef _DEBUG
	// ImGUiを表示する
	DisplayImGui();
#endif // _DEBUG
}

void GoalAnimComp::DisplayImGui()
{
	ImGui::Begin("GoalAnim");
	// 旗の位置座標を設定
	if (ImGui::TreeNode("Flag Transform")) {
		transform_->ImGuiWidget();
		ImGui::TreePop();
	}

	// ゴールカメラ
	if (ImGui::TreeNode("GoalCamera")) {
		// オフセット
		ImGui::DragFloat3("OffSet - Translate", &offsetTranslate_.x, 0.05f);
		ImGui::DragFloat3("OffSet - Rotate", &offsetRotate_.x, 0.05f);
		ImGui::TreePop();
	}

	// ゴールアニメーションの再生
	if (ImGui::Button("PlayGoalAnim")) {
		PlayGoalAnim();
	}

	ImGui::End();
}

void GoalAnimComp::PlayGoalAnim()
{
	// プレイフラグがtrueではないとき
	if (!isPlay_) {
		if (playerModel_ != nullptr) {
			// クリアアニメーション再生
			playerModel_->GetComponent<PlayerAnimComp>()->GetAnimManager()->SetNextAnimation(PlayerBehavior::kClear, false);
		}

		// フラグをtrue
		isPlay_ = true;
	}
}
