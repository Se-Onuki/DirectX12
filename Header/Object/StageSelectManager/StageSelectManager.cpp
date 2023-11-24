#include "StageSelectManager.h"

void StageSelectManager::Init()
{
	// インスタンス取得
	gv_ = GlobalVariables::GetInstance(); // 調整項目クラス
	input_ = Input::GetInstance(); // 入力用

	// 調整項目クラスから値を読む
	ApplyItem();
	
	// UI関係初期化
	ui_.Init();

}

void StageSelectManager::Update([[maybe_unused]]float deltaTime)
{
	// キーボードの入力取得
	static const auto* const keyBoard = input_->GetDirectInput();

	// キー入力をすると選択番号を変更
	if (not ui_.GetIsPlayingAnim()) {
		if (keyBoard->IsTrigger(DIK_RIGHTARROW)) {
			if (selectedStageNumber_ < maxLevelCount_ - 1)
				selectedStageNumber_++;
			else
				selectedStageNumber_ = 0;

			ui_.SetIsRight(true);
		}
		else if (keyBoard->IsTrigger(DIK_LEFTARROW)) {
			if (selectedStageNumber_ > 0)
				selectedStageNumber_--;
			else
				selectedStageNumber_ = maxLevelCount_ - 1;

			ui_.SetIsRight(false);
		}
	}

	// UIの更新処理
	ui_.Update(deltaTime);

}

void StageSelectManager::Draw([[maybe_unused]] const Camera3D& camera)
{
	
}

void StageSelectManager::SpriteDraw()
{
	// UIを描画
	ui_.Draw();
}

void StageSelectManager::DisplayImGui()
{
	// ImGuiを描画
	ImGui::Begin("StageSelectManager");
	// 選択中ステージ番号の表示
	ImGui::InputInt("SelectedStageNumber", &selectedStageNumber_);
	ImGui::End();

	// UI関係のImGuiの表示
	ui_.DisplayImGui();
}

void StageSelectManager::AddItem()
{
	
}

void StageSelectManager::ApplyItem()
{
	// 調整項目クラスから値を取得
	maxLevelCount_ = gv_->Get<int>("LevelList", "LevelCount"); // ステージ最大数
}
