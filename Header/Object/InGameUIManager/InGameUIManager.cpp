#include "InGameUIManager.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../../Header/Object/StageSelectManager/StageSelectManager.h"
#include "../LevelElementManager.h"

// 静的なメンバ変数の実体を宣言
uint32_t InGameUIManager::dontSpinSE_ = 0u;

void InGameUIManager::Init(int maxStarCount)
{
	// 最大星数を取得
	maxStarCount_ = maxStarCount;

	// 選択ステージ番号の取得
	selectedStageNumber_ = StageSelectManager::GetInstance()->GetSelectedStageNumber();

	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_ = std::make_unique<TutorialUI>();
		tutorialUI_->Init("TutorialUI");
	}
	else if (selectedStageNumber_ == 2) {
		// 操作方法UIのリセット
		replayTutorialUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/ReturnTutorial.png")));
		replayTutorialUI_.position_ = { 335.0f, 55.0f };
		replayTutorialUI_.scale_ = { 0.0f, 85.0f };
		replayTutorialUI_.anchorPoint_ = { 0.0f, 0.5f };
		replayTutorialUI_.sprite_->SetTexDiff({ 0.0f, 170.0f });
		replaySpriteAnimTimer_.Start(1.0f);
	}

	// 最大星数分スプライトを追加
	for (int i = 0; i < maxStarCount_; i++) {
		// 新しいUIを生成
		std::unique_ptr<StarUI> newUI = std::make_unique<StarUI>();
		// 生成したUIを配列に追加
		std::string addName = "Star" + std::to_string(i);
		newUI->Init(addName);
		stars_.push_back(std::move(newUI));
	}

	// 操作方法UIのリセット
	controllUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/Controll/Controller_NoSpin.png")));
	controllUI_.position_ = { 0.0f, 0.0f };
	controllUI_.scale_ = { 512.0f, 720.0f };
	controllUI_.anchorPoint_ = { 0.0f, 0.0f };
	// スピン時の操作方法UIのリセット
	spinControllUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/Controll/Controller_Spin.png")));
	spinControllUI_.position_ = { 0.0f, 0.0f };
	spinControllUI_.scale_ = { 512.0f, 720.0f };
	spinControllUI_.anchorPoint_ = { 0.0f, 0.0f };

	// チェックポイント更新UIのリセット
	updateCheckPointUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/UpdateCheckPoint.png")));
	updateCheckPointUI_.position_ = { 1050.0f, 350.0f };
	updateCheckPointUI_.scale_ = { 256.0f, 64.0f };
	updateCheckPointUI_.anchorPoint_ = { 0.5f, 0.5f };

	if (dontSpinSE_ == 0u) {
		dontSpinSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/dontSpin.wav");
	}
}

void InGameUIManager::Update(float deltaTime)
{
	if (Input::GetInstance()->GetDirectInput()->IsPress(DIK_X)&& 
		Input::GetInstance()->GetDirectInput()->IsTrigger(DIK_P)) {
		if (uiAlpha_ != 0.0f) {
			uiAlpha_ = 0.0f;
		}
		else {
			uiAlpha_ = 1.0f;
		}
	}

	// 最大星数分ループ
	int count = 0;
	for (auto& star : stars_) {
		star->Update(deltaTime);
		if (count == 0) {
			star->position_ = starUIsStartingPoint_ + shakeVector_;
		}
		else {
			star->position_ = starUIsStartingPoint_;
			star->position_ = { starUIsStartingPoint_.x + (starUILineSpace_ * count) + shakeVector_.x, starUIsStartingPoint_.y + shakeVector_.y };
		}
		star->overrapSpriteAlpha_ = uiAlpha_;
		count++;
	}

	// 星の振動を呼び出す
	ShakingStar();

	// 振動演出用タイマー更新
	shakeAnimTimer_.Update(deltaTime);

	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_->Update(deltaTime);
		tutorialUI_->alphaMagnification_ = uiAlpha_;
	}
	else if (selectedStageNumber_ == 2) {
		replayTutorialUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f,uiAlpha_ });

		switch (replayAnimCount_)
		{
			case 0:
				if (not replaySpriteAnimTimer_.IsFinish()) {
					replayTutorialUI_.scale_.x = SoLib::Lerp<float>(0.0f, 426.0f, SoLib::easeOutQuad(replaySpriteAnimTimer_.GetProgress()));
					replayTutorialUI_.sprite_->SetTexDiff({SoLib::Lerp<float>(0.0f, 853.0f, SoLib::easeOutQuad(replaySpriteAnimTimer_.GetProgress())), 170.0f });

				}
				else {
					replayTutorialUI_.scale_.x = 426.0f;
					replaySpriteAnimTimer_.Start(5.0f);
					replayAnimCount_++;
				}
				break;
			case 1:
				if (replaySpriteAnimTimer_.IsFinish()) {
					replayTutorialUI_.scale_.x = 426.0f;
					replaySpriteAnimTimer_.Start(1.0f);
					replayAnimCount_++;
				}
				break;
			case 2:
				if (not replaySpriteAnimTimer_.IsFinish()) {
					replayTutorialUI_.scale_.x = SoLib::Lerp<float>(426.0f, 0.0f, SoLib::easeOutQuad(replaySpriteAnimTimer_.GetProgress()));
					replayTutorialUI_.sprite_->SetTexDiff({ SoLib::Lerp<float>(853.0f, 0.0f, SoLib::easeOutQuad(replaySpriteAnimTimer_.GetProgress())), 170.0f });
				}
				else {
					replayTutorialUI_.scale_.x = 0.0f;
				}
				break;

		}
		// タイマー更新
		replaySpriteAnimTimer_.Update(deltaTime);
	}

	if (Input::GetInstance()->GetInputType() == Input::InputType::kKeyBoard) {
		controllUI_.sprite_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Controll/keyboard_NoSpin.png"));
		spinControllUI_.sprite_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Controll/keyboard_Spin.png"));
	}
	else {
		controllUI_.sprite_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Controll/Controller_NoSpin.png"));
		spinControllUI_.sprite_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Controll/Controller_Spin.png"));
	}

	// プレイヤーコンポーネントがセットされている場合
	if (playerComp_ != nullptr) {
		if (playerComp_->GetState()->GetState() == PlayerBehavior::kRotating) {
			isSpining_ = true;
		}
		else {
			isSpining_ = false;
		}
	}

	// 
	if (playCheckPointAnim_) {
		switch (checkPointAnimCount_)
		{
		case 0:
			if (not checkPointAnimTimer_.IsFinish()) {
				updateCheckPointUI_.position_.y = SoLib::Lerp<float>(350.0f, 300.0f, SoLib::easeOutQuad(checkPointAnimTimer_.GetProgress()));
				alpha_ = SoLib::Lerp<float>(0.0f, 1.0f, SoLib::easeOutQuad(checkPointAnimTimer_.GetProgress()));
			}
			else {
				updateCheckPointUI_.position_.y = 300.0f;
				alpha_ = 1.0f;
				checkPointAnimTimer_.Start(2.0f);
				checkPointAnimCount_++;
			}
			break;
		case 1:
			if (not checkPointAnimTimer_.IsFinish()) {
				updateCheckPointUI_.position_.y = SoLib::Lerp<float>(300.0f, 285.0f, SoLib::easeOutQuad(checkPointAnimTimer_.GetProgress()));
			}
			else {

				updateCheckPointUI_.position_.y = 285.0f;
				alpha_ = 1.0f;
				checkPointAnimTimer_.Start(1.0f);
				checkPointAnimCount_++;
			}
			break;
		case 2:
			if (not checkPointAnimTimer_.IsFinish()) {
				alpha_ = SoLib::Lerp<float>(1.0f, 0.0f, SoLib::easeOutQuad(checkPointAnimTimer_.GetProgress()));
			}
			else {
				alpha_ = 0.0f;
				playCheckPointAnim_ = false;
			}
			break;
		}

		// 
		checkPointAnimTimer_.Update(deltaTime);
	}

	controllUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f, uiAlpha_ });
	updateCheckPointUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ * uiAlpha_ });

	Entity* goal = nullptr;
	if (LevelElementManager::GetInstance()->GetGoalList().size()) {
		goal = LevelElementManager::GetInstance()->GetGoalList().front();

		// ゴール状態である場合
		if (goal->GetComponent<GoalAnimComp>()->GetIsPlay()) {
			if (uiAlpha_ > 0.0f) {
				uiAlpha_ -= 0.025f;
			}
			else {
				uiAlpha_ = 0.0f;
			}
			
		}
	}

#ifdef _DEBUG
	ImGui::Begin("InGameUI");
	if (ImGui::TreeNode("StarUI")) {
		ImGui::DragFloat2("StarUIStartingPoint", &starUIsStartingPoint_.x, 1.0f);
		ImGui::DragFloat("StarUIsLineSpace", &starUILineSpace_, 1.0f);
		// 最大星数分ループ
		for (auto& star : stars_) {
			star->DisplayImGui();
		}

		ImGui::InputInt("AddType", &imGuiAddType_);
		if (ImGui::Button("AddStar")) {
			AddStar(imGuiAddType_);
		}
		if (ImGui::Button("SetStar")) {
			SetStar(imGuiAddType_);
		}
		ImGui::TreePop();
	}

	controllUI_.DisplayImGui("ControllUI");
	spinControllUI_.DisplayImGui("spinControllUI");
	updateCheckPointUI_.DisplayImGui("updateCheckPointUI");
	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_->DisplayImGui();
	}
	else if (selectedStageNumber_ == 2) {
		replayTutorialUI_.DisplayImGui("replayTutorialUI");
	}

	ImGui::DragFloat("UI - Alpha", &uiAlpha_, 0.01f, 0.0f, 1.0f);

	ImGui::Checkbox("isSpining", &isSpining_);

	ImGui::End();
#endif // _DEBUG

}

void InGameUIManager::Draw()
{
	// 最大星数分ループ
	for (auto& star : stars_) {
		star->Draw();
	}
	// 操作方法UI描画
	if (!isSpining_) {
		controllUI_.Draw();
	}
	else {
		spinControllUI_.Draw();
	}

	updateCheckPointUI_.Draw();

	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_->Draw();
	}
	else if (selectedStageNumber_ == 2) {
		replayTutorialUI_.Draw();
	}

	
}

void InGameUIManager::AddStar(int p0m)
{
	updateCheckPointUI_.position_ = { 1050.0f, 350.0f };
	checkPointAnimCount_ = 0;
	checkPointAnimTimer_.Start(1.0f);
	playCheckPointAnim_ = true;

	if (p0m >= 0) {
		// 最大星数分ループ
		std::list<std::unique_ptr<StarUI>>::iterator itr = stars_.begin();
		for (; itr != stars_.end(); itr++) {
			auto& star = *itr;
			if (!star->GetIsFilled()) {
				star->SetBehavior(StarUI::kAppear);
				break;
			}
		}
	}
	else {
		// 最大星数分ループ
		std::list<std::unique_ptr<StarUI>>::reverse_iterator itr = stars_.rbegin();
		for (; itr != stars_.rend(); itr++) {
			auto& star = *itr;
			if (star->GetIsFilled()) {
				star->SetBehavior(StarUI::kDisappear);
				break;
			}
		}
	}
}

void InGameUIManager::SetStar(int count)
{
	// 最大星数分ループ
	std::list<std::unique_ptr<StarUI>>::iterator itr = stars_.begin();
	for (; itr != stars_.end(); itr++) {
		auto& star = *itr;
		star->Reset();
	}

	int c = 0;
	for (auto& star : stars_) {
		if (c < count) {
			star->SetBehavior(StarUI::kAppear);
		}
		c++;
	}
}

void InGameUIManager::ShakeStar()
{
	// 振動を有効にする
	blurTrigger_ = true;

	// 効果音を鳴らす
	Audio::GetInstance()->PlayWave(dontSpinSE_, false, 0.45f);

	// 振動アニメーション開始
	shakeAnimTimer_.Start(kShakeTime);
}

void InGameUIManager::ShakingStar()
{
	// 振動トリガーが有効なら
	if (blurTrigger_) {
		// 振動アニメーションタイマーが終了していなければ
		if (!shakeAnimTimer_.IsFinish()) {
			// ｘ、ｙそれぞれの振動ベクトルをランダムに求める
			shakeVector_.x = Random::GetRandom<float>(-shakeRange_, shakeRange_);
			shakeVector_.y = Random::GetRandom<float>(-shakeRange_, shakeRange_);

			// 振動範囲を線形補間で徐々に狭めていく
			shakeRange_ = SoLib::Lerp<float>(kShakeRange, 0.0f, shakeAnimTimer_.GetProgress());
		}
		else {
			// 終了したらベクトル初期化、範囲初期化
			shakeVector_ = { 0.0f, 0.0f };
			shakeRange_ = kShakeRange;

			// トリガー無効
			blurTrigger_ = false;
		}
	}
}
