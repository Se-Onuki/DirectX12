#include "PoseManager.h"
#include "../../../Engine/DirectBase/Base/Audio.h"

uint32_t PoseManager::deploySE_ = 0u;
uint32_t PoseManager::closeSE_ = 0u;
uint32_t PoseManager::nextSE_ = 0u;
uint32_t PoseManager::retrySE_ = 0u;
uint32_t PoseManager::returnSE_ = 0u;

void PoseManager::Init()
{
	// 入力取得
	input_ = Input::GetInstance();

	// ポーズ画面用UI
	//if (poseUI_.sprite_) {
	poseUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/ingameStopUI.png")));
	//}
	poseUI_.position_ = { 1280.0f / 2.0f, 360.0f + (360.0f * 2.0f) };
	poseUI_.scale_ = { 500.0f, 500.0f };
	poseUI_.anchorPoint_ = { 0.5f, 0.5f };
	poseUI_.sprite_->SetTexOrigin({ 0.0f, 0.0f });
	poseUI_.sprite_->SetTexDiff({ 500.0f, 500.0f });
	poseUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	// 実行中カテゴリリセット
	executioningCategory_ = kNone;

	// 始端値と終端値の設定
	poseUIStartPos_ = poseUI_.position_;
	poseUIEndPos_ = { 1280.0f / 2.0f, 360.0f };
	poseUIStartColor_ = { 1.0f, 1.0f, 1.0f, 0.0f };
	poseUIEndColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	if (deploySE_ == 0u) {
		deploySE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/openPoseMenu.wav");
	}
	if (closeSE_ == 0u) {
		closeSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/closePoseMenu.wav");
	}
	if (nextSE_ == 0u) {
		nextSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/nextCategory.wav");
	}
	if (retrySE_ == 0u) {
		retrySE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/returnCheckPoint.wav");
	}
	if (returnSE_ == 0u) {
		returnSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/UI/returnStageSelect.wav");
	}

	// メニューを閉じていない
	close_ = false;

	// 演出用タイマースタート
	timer_.Start(0.5f);
}

void PoseManager::Update(float deltaTime)
{
	deltaTime;
	if (isDisplay_) {
		// 入力によって選択されているカテゴリを変更
		if (input_->GetDirectInput()->IsTrigger(DIK_DOWN) || input_->GetXInput()->IsTrigger(KeyCode::DPAD_DOWN)) {
			Audio::GetInstance()->PlayWave(nextSE_, false, 0.5f);
			if (selectedCategory_ < kReturnStageSelect)
				selectedCategory_++;
			else
				selectedCategory_ = kResume;
		}
		else if (input_->GetDirectInput()->IsTrigger(DIK_UP) || input_->GetXInput()->IsTrigger(KeyCode::DPAD_UP)) {
			Audio::GetInstance()->PlayWave(nextSE_, false, 0.5f);
			if (selectedCategory_ > kResume)
				selectedCategory_--;
			else
				selectedCategory_ = kReturnStageSelect;
		}

		// Bボタン入力でメニューを閉じる
		if (input_->GetDirectInput()->IsTrigger(DIK_ESCAPE) || input_->GetXInput()->IsTrigger(KeyCode::B)) {
			Audio::GetInstance()->PlayWave(closeSE_, false, 0.5f);
			CloseMenu();
		}

		if (input_->GetDirectInput()->IsTrigger(DIK_SPACE) || input_->GetXInput()->IsTrigger(KeyCode::A)) {
			PressAButton();
		}
	}

	if (isActive_) {
		if (!timer_.IsFinish()) {
			// ポーズUiをイージングで移動
			if (close_) {
				poseUI_.position_ = SoLib::Lerp(poseUIStartPos_, poseUIEndPos_, SoLib::easeInQuad(timer_.GetProgress()));
				poseUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f, SoLib::Lerp(poseUIStartColor_.w, poseUIEndColor_.w, SoLib::easeInQuad(timer_.GetProgress())) });
			}
			else {
				poseUI_.position_ = SoLib::Lerp(poseUIStartPos_, poseUIEndPos_, SoLib::easeOutQuad(timer_.GetProgress()));
				poseUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f, SoLib::Lerp(poseUIStartColor_.w, poseUIEndColor_.w, SoLib::easeOutQuad(timer_.GetProgress())) });
			}
		}
		else {
			// 表示されていなければ
			if (!isDisplay_) {
				poseUI_.position_ = poseUIEndPos_;
				poseUIEndPos_ = poseUIStartPos_;
				poseUIStartPos_ = poseUI_.position_;
				poseUI_.sprite_->SetColor(poseUIEndColor_);
				poseUIEndColor_ = poseUIStartColor_;
				poseUIStartColor_ = poseUI_.sprite_->GetColor();
				isDisplay_ = true;
			}

			// メニューを閉じる状態ならば
			if (close_) {
				poseUI_.position_ = poseUIEndPos_;
				poseUI_.sprite_->SetColor(poseUIEndColor_);
				// 操作できない状態に
				isDisplay_ = false;
				isActive_ = false;
			}
		}

		// タイマー更新
		timer_.Update(deltaTime);
	}

	poseUI_.sprite_->SetTexOrigin({ 500.0f * (float)selectedCategory_, 0.0f });
}

void PoseManager::Draw()
{
	if (isActive_) {
		poseUI_.Draw();
	}
}

void PoseManager::DeplayPoseMenu()
{
	if (!isActive_) {
		// 再度初期化
		Init();
		Audio::GetInstance()->PlayWave(deploySE_, false, 0.5f);
		// 表示
		isActive_ = true;
	}
}

void PoseManager::CloseMenu()
{
	// 演出用タイマースタート
	timer_.Start(0.5f);
	// 操作できない状態に
	isDisplay_ = false;
	// メニューを閉じる
	close_ = true;
}

void PoseManager::PressAButton()
{
	// 選択カテゴリによって処理を分岐
	switch (selectedCategory_) {
	case PoseManager::kResume:
		Audio::GetInstance()->PlayWave(closeSE_, false, 0.5f);
		// メニューを閉じる
		CloseMenu();
		break;
	case PoseManager::kReturnCheckPoint:
		// チェックポイントに戻るように指示
		executioningCategory_ = kReturnCheckPoint;
		Audio::GetInstance()->PlayWave(retrySE_, false, 0.5f);
		// 操作不可能な状態に
		CloseMenu();
		break;
	case PoseManager::kRetry:
		// リトライを指示
		executioningCategory_ = kRetry;
		Audio::GetInstance()->PlayWave(retrySE_, false, 0.5f);
		CloseMenu();
		break;
	case PoseManager::kReturnStageSelect:
		// ステージ選択画面に戻るように指示
		executioningCategory_ = kReturnStageSelect;
		Audio::GetInstance()->PlayWave(returnSE_, false, 0.5f);
		CloseMenu();
		break;
	default:
		break;
	}
}
