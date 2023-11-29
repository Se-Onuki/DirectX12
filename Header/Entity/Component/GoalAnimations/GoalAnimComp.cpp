#include "GoalAnimComp.h"
#include "../ModelComp.h"
#include "../PlayerAnimComp.h"
#include "../../../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../../../Engine/DirectBase/Model/ModelManager.h"
#include "../../../Object/StageSelectManager/StageSelectManager.h"
#include "../../../Object/Particle/ParticleEmitterManager.h"

void GoalAnimComp::Init()
{
	// モデルコンポーネントの追加
	object_->AddComponent<ModelComp>();
	// ゴール
	object_->GetComponent<ModelComp>()->AddBone("Base", ModelManager::GetInstance()->GetModel("Flag"));

	// インスタンス取得
	gv_ = GlobalVariables::GetInstance();					 // 調整項目クラス
	cameraManager_ = CameraManager::GetInstance();			 // カメラマネージャー
	stageSelectManager_ = StageSelectManager::GetInstance(); // ステージ選択マネージャー

	// ゴール時のカメラを生成
	goalCamera_ = cameraManager_->AddCamera("goalCamera");

	// アイテム追加
	AddItem();
	// アイテムを読み込む
	ApplyItem();
}

void GoalAnimComp::Update()
{
	if (isPlay_) {
		/// ゴール時のカメラのオフセットを設定
		// ゴール時カメラの回転角を設定
		goalCamera_->translation_ = playerModel_->transform_.translate;
		// 角度合わせのために強制的に設定
		playerModel_->transform_.rotate.y = playerRotateY_;
		goalCamera_->rotation_ = offsetRotate_;

		// オフセット回転角を加算した回転行列を生成する
		Matrix4x4 rotateMat = Matrix4x4::EulerRotate(goalCamera_->rotation_);
		// オフセットを求める
		Vector3 offset = MatrixToVector(offsetTranslate_, rotateMat);
		// 求めた値を代入する
		goalCamera_->translation_ += offset;

		if (playerModel_->GetComponent<PlayerAnimComp>()->GetAnimManager()->GetAnimationProgress() >= 1.0f) {
			playerModel_->GetComponent<PlayerAnimComp>()->GetAnimManager()->SetNextAnimation(PlayerBehavior::kClearBleak, true);
			// ゴール演出が終了したことを告げる
			isEnd_ = true;
		}
	}

#ifdef _DEBUG
	// ImGUiを表示する
	DisplayImGui();
#endif // _DEBUG
}

void GoalAnimComp::DisplayImGui()
{
#ifdef _DEBUG
	ImGui::Begin("GoalAnim");
	// 旗の位置座標を設定
	if (ImGui::TreeNode("Flag Transform")) {
		transform_->ImGuiWidget("");
		ImGui::TreePop();
	}

	// ゴールカメラ
	if (ImGui::TreeNode("GoalCamera")) {
		// オフセット
		ImGui::DragFloat3("OffSet - Translate", &offsetTranslate_.x, 0.05f);
		ImGui::DragFloat3("OffSet - Rotate", &offsetRotate_.x, 0.05f);
		ImGui::DragFloat("PlayerRotateY", &playerRotateY_, 0.001f, -(float)std::numbers::pi, (float)std::numbers::pi);
		ImGui::TreePop();
	}

	// ゴールアニメーションの再生
	if (ImGui::Button("PlayGoalAnim")) {
		PlayGoalAnim();
	}

	if (ImGui::TreeNode("ParamMenu")) {

		// 情報追加を行うステージ番号の設定
		ImGui::InputInt("Add To Stage Number", &imGuiAddToStageNumber_);
		// 情報を読み込むステージ番号の指定
		ImGui::InputInt("Read To Stage Number", &ImGuiReadToStageNumber_);

		// パラメーターを指定した値に追加
		if (ImGui::Button("AddItem"))
			AddItem(imGuiAddToStageNumber_);
		// パラメーターを指定した値にセット
		if (ImGui::Button("SetItem"))
			SetItem(imGuiAddToStageNumber_);
		// パラメーターを指定した値から読み込む
		if (ImGui::Button("ApplyItem"))
			ApplyItem(ImGuiReadToStageNumber_);
		// 追加パラメーターをセーブ
		if (ImGui::Button("SaveFiles"))
			gv_->SaveFile("GoalAnimParameters");

		ImGui::TreePop();
	}

	// 再生状態取得、変更
	ImGui::Checkbox("isPlay", &isPlay_);
	ImGui::Checkbox("isEnd", &isEnd_);

	ImGui::End();

#endif // _DEBUG

}

void GoalAnimComp::SetPlayerModel(Entity* entity)
{
	playerModel_ = entity;
	pEmitter_ = ParticleEmitterManager::GetInstance()->CreateEmitter<StarParticle>("PlayerLing");
	pEmitter_->targetTransform_ = &playerModel_->transform_;
	pEmitter_->offset_ = { 0.0f, 0.5f, 0.0f };
}

void GoalAnimComp::PlayGoalAnim()
{
	// プレイフラグがtrueではないとき
	if (!isPlay_) {
		if (playerModel_ != nullptr) {
			// クリアアニメーション再生
			playerModel_->GetComponent<PlayerAnimComp>()->GetAnimManager()->SetNextAnimation(PlayerBehavior::kClear, false);
			// パーティクルを強制終了
			if (pEmitter_) {
				pEmitter_->Finish();
			}

			/// ゴール時のカメラのオフセットを設定
			// ゴール時カメラの回転角を設定
			goalCamera_->translation_ = playerModel_->transform_.translate;
			// 角度合わせのために強制的に設定
			playerModel_->transform_.rotate.y = playerRotateY_;
			goalCamera_->rotation_ = offsetRotate_;

			// オフセット回転角を加算した回転行列を生成する
			Matrix4x4 rotateMat = Matrix4x4::EulerRotate(goalCamera_->rotation_);
			// オフセットを求める
			Vector3 offset = MatrixToVector(offsetTranslate_, rotateMat);
			// 求めた値を代入する
			goalCamera_->translation_ += offset;
		}

		// カメラのアニメーションを再生する用指示
		cameraManager_->GetCameraAnimManager()->Play(goalCamera_, 1.5f, SoLib::easeInOutQuad);

		// フラグをtrue
		isPlay_ = true;
	}
}

const Vector3 GoalAnimComp::MatrixToVector(const Vector3 &v, const Matrix4x4 &m)
{
	// 結果格納用
	Vector3 result;

	// 生成処理
	result.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (1.0f * m.m[3][0]);
	result.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (1.0f * m.m[3][1]);
	result.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (1.0f * m.m[3][2]);
	float w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (1.0f * m.m[3][3]);

	assert(w != 0.0f);

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

void GoalAnimComp::AddItem(int num)
{
	// ステージ番号取得
	std::string groupName = "GoalAnimParameters";
	std::string stageNumber = std::to_string(num);
	// 未設定の場合は選択中のステージ番号から値を読む
	if (num == 999) {
		stageNumber = std::to_string(stageSelectManager_->GetSelectedStageNumber());
	}

	// 各パラメータ設定
	gv_->AddValue(groupName, "OffsetTranslate" + stageNumber, offsetTranslate_); // オフセット位置
	gv_->AddValue(groupName, "OffsetRotate" + stageNumber, offsetRotate_);		 // 回転角
	gv_->AddValue(groupName, "PlayerRotateY" + stageNumber, playerRotateY_);	 // プレイヤー向き
}

void GoalAnimComp::SetItem(int num)
{
	// ステージ番号取得
	std::string groupName = "GoalAnimParameters";
	std::string stageNumber = std::to_string(num);
	// 未設定の場合は選択中のステージ番号から値を読む
	if (num == 999) {
		stageNumber = std::to_string(stageSelectManager_->GetSelectedStageNumber());
	}
	// 各パラメータ設定
	gv_->SetValue(groupName, "OffsetTranslate" + stageNumber, offsetTranslate_); // オフセット位置
	gv_->SetValue(groupName, "OffsetRotate" + stageNumber, offsetRotate_);		 // 回転角
	gv_->SetValue(groupName, "PlayerRotateY" + stageNumber, playerRotateY_);	 // プレイヤー向き
}

void GoalAnimComp::ApplyItem(int num)
{
	// ステージ番号取得
	std::string groupName = "GoalAnimParameters";
	std::string stageNumber = std::to_string(num);
	// 未設定の場合は選択中のステージ番号から値を読む
	if (num == 999) {
		stageNumber = std::to_string(stageSelectManager_->GetSelectedStageNumber());
	}
	// 各パラメータ取得
	offsetTranslate_ = gv_->Get<Vector3>(groupName, "OffsetTranslate" + stageNumber); // オフセット位置
	offsetRotate_ = gv_->Get<Vector3>(groupName, "OffsetRotate" + stageNumber);		  // 回転角
	playerRotateY_ = gv_->Get<float>(groupName, "PlayerRotateY" + stageNumber);		  // プレイヤー向き
}
