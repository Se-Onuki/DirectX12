#include "StageSelectManager.h"
#include "../../../Engine/DirectBase/Base/Audio.h"
#include "../../../Header/Object/LevelElementManager.h"

// 静的なメンバ変数の実体を宣言
uint32_t StageSelectManager::selectStageSE_ = 0u;

void StageSelectManager::Init()
{
	// インスタンス取得
	gv_ = GlobalVariables::GetInstance(); // 調整項目クラス
	input_ = Input::GetInstance(); // 入力用

	if (selectStageSE_ == 0u) {
		selectStageSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/SelectStage.wav");
	}

	// 調整項目クラスから値を読む
	ApplyItem();

	// UI関係初期化
	ui_.Init();

	if (LevelElementManager::GetInstance()->isCleared_) {
		selectedStageNumber_++;
		LevelElementManager::GetInstance()->isCleared_ = false;
		selectedStageNumber_ = std::clamp(selectedStageNumber_, 0, maxLevelCount_ - 1);
		ui_.SetIsRight(true);
	}

}

void StageSelectManager::Update([[maybe_unused]] float deltaTime)
{
	// キーボードの入力取得
	static const auto *const keyBoard = input_->GetDirectInput();

	// キー入力をすると選択番号を変更
	if (not ui_.GetIsPlayingAnim()) {
		if (keyBoard->IsTrigger(DIK_RIGHTARROW) || input_->GetXInput()->IsTrigger(KeyCode::DPAD_RIGHT)) {
			Audio::GetInstance()->PlayWave(selectStageSE_, false, 0.65f);
			if (selectedStageNumber_ < maxLevelCount_ - 1)
				selectedStageNumber_++;
			else
				selectedStageNumber_ = 0;

			ui_.SetIsRight(true);
		}
		else if (keyBoard->IsTrigger(DIK_LEFTARROW) || input_->GetXInput()->IsTrigger(KeyCode::DPAD_LEFT)) {
			Audio::GetInstance()->PlayWave(selectStageSE_, false, 0.65f);
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

void StageSelectManager::Draw([[maybe_unused]] const Camera3D &camera)
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
