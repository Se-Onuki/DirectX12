#include "TutorialUI.h"

void TutorialUI::Init(const std::string& id)
{
	// ID取得
	id_ = id;
	// スプライトのインスタンスを生成
	tutorialUI_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/Tutorial_1.png")));
	tutorialKeyUI_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/TutorialKey_1.png")));
}

void TutorialUI::Update(float deltaTime)
{
	switch (progress_)
	{
	case TutorialUI::kMove:
		tutorialUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/Tutorial_1.png"));
		tutorialKeyUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/TutorialKey_1.png"));
		break;
	case TutorialUI::kEnableSpin:
		tutorialUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/Tutorial_2.png"));
		tutorialKeyUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/TutorialKey_2.png"));
		break;
	case TutorialUI::kSpin:
		tutorialUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/Tutorial_3.png"));
		tutorialKeyUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/TutorialKey_3.png"));
		break;
	case TutorialUI::kDisableSpin:
		tutorialUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/Tutorial_4.png"));
		tutorialKeyUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/TutorialKey_4.png"));
		break;
	case TutorialUI::kGoal:
		tutorialUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/Tutorial_5.png"));
		tutorialKeyUI_->SetTextureHaundle(TextureManager::Load("UI/TD2_3week_2/InGame/Tutorial/TutorialKey_5.png"));
		break;
	}

	// ふるまいを変更するリクエストがあればTrue
	if (behaviorRequest_) {
		// ふるまい変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case TutorialUIBehavior::kStay:
			// 待機アニメーション初期化
			StayInit();
			break;
		case TutorialUIBehavior::kAppear:
			// 表示アニメーション初期化
			AppearInit();
			break;
		case TutorialUIBehavior::kDisappear:
			// 非表示アニメーション初期化
			DisappearInit();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	// 各振る舞いごとの更新を実行
	switch (behavior_) {
	case TutorialUIBehavior::kStay:
		// 待機アニメーション初期化
		StayUpdate();
		break;
	case TutorialUIBehavior::kAppear:
		// 行うアニメーション自体は同一なので同じ関数を使う
		AppearUpdate();
		break;
	case TutorialUIBehavior::kDisappear:
		// 行うアニメーション自体は同一なので同じ関数を使う
		DisappearUpdate();
		break;
	}

	// タイマー更新
	animTimer_.Update(deltaTime);

#ifdef _DEBUG
	// ImGuiの描画
	DisplayImGui();
#endif // _DEBUG
}

void TutorialUI::Draw()
{
	if (isController_) {
		// 背景の描画
		tutorialUI_->SetPosition(position_);
		tutorialUI_->SetScale(scale_);
		tutorialUI_->SetPivot(anchorPoint_);
		tutorialUI_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ * alphaMagnification_ });
		tutorialUI_->Draw();
	}
	else {
		// スプライトの描画
		tutorialKeyUI_->SetPosition(position_);
		tutorialKeyUI_->SetScale(scale_);
		tutorialKeyUI_->SetPivot(anchorPoint_);
		tutorialKeyUI_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ * alphaMagnification_ });
		tutorialKeyUI_->Draw();
	}
}

void TutorialUI::DisplayImGui()
{
	if (ImGui::TreeNode(id_.c_str())) {
		ImGui::DragFloat2("Position", &position_.x, 1.0f);
		ImGui::DragFloat2("Scale", &scale_.x, 1.0f);
		ImGui::DragFloat2("AnchorPoint", &anchorPoint_.x, 0.01f);

		ImGui::RadioButton("Stay", &imGuiSelectBehavior_, kStay);
		ImGui::SameLine();
		ImGui::RadioButton("Appear", &imGuiSelectBehavior_, kAppear);
		ImGui::SameLine();
		ImGui::RadioButton("Disappear", &imGuiSelectBehavior_, kDisappear);

		ImGui::Checkbox("IsController", &isController_);

		if (ImGui::Button("Change Behavior")) {
			behaviorRequest_ = (TutorialUIBehavior)imGuiSelectBehavior_;
		}

		ImGui::SliderInt("progress", &progress_, kMove, kGoal);

		ImGui::TreePop();
	}
}

void TutorialUI::DisplayNextTutorial(int progress)
{
	// 次の進捗と一致しない場合はそれを代入する
	if (nextProgress_ != progress_) {
		progress_ = nextProgress_;
	}
	nextProgress_ = progress;
	behaviorRequest_ = kDisappear;
	changeTutorial_ = true;
}

void TutorialUI::StayInit()
{
	// 設定された終端値に瞬時に合わせる
	position_ = endTranslate_;
	alpha_ = endAlpha_;

	// タイマーを強制停止
	animTimer_.Clear();
}

void TutorialUI::StayUpdate()
{
	// 更新処理は今のところ記述なし
}

void TutorialUI::AppearInit()
{
	// 現段階のメンバ変数のパラメーターを取得
	startTranslate_ = position_; // 大きさ始端値
	endTranslate_ = { 640.0f, 160.0f }; // 大きさ終端値
	startAlpha_ = alpha_; // 大きさ始端値
	endAlpha_ = 1.0f; // 大きさ終端値

	// タイマーを開始
	animTimer_.Start(kAppearTime_);
}

void TutorialUI::AppearUpdate()
{
	if (not animTimer_.IsFinish()) {
		// パラメーターを線形補間で変更する
		position_ = SoLib::Lerp(startTranslate_, endTranslate_, SoLib::easeInOutQuad(animTimer_.GetProgress()));
		alpha_ = SoLib::Lerp(startAlpha_, endAlpha_, SoLib::easeOutQuad(animTimer_.GetProgress()));
		isChanging_ = true;
	}
	else {
		isChanging_ = false;
		// 次の行動リクエストに待機アニメーションを設定
		behaviorRequest_ = kStay;
		// 表示中
		isDisplay_ = true;
	}
}

void TutorialUI::DisappearInit()
{
	// 現段階のメンバ変数のパラメーターを取得
	startTranslate_ = position_; // 大きさ始端値
	endTranslate_ = { 640.0f, -80.0f }; // 大きさ終端値
	startAlpha_ = alpha_; // 大きさ始端値
	endAlpha_ = 0.0f; // 大きさ終端値

	// タイマーを開始
	animTimer_.Start(kAppearTime_);
}

void TutorialUI::DisappearUpdate()
{
	if (not animTimer_.IsFinish()) {
		// パラメーターを線形補間で変更する
		position_ = SoLib::Lerp(startTranslate_, endTranslate_, SoLib::easeInOutQuad(animTimer_.GetProgress()));
		alpha_ = SoLib::Lerp(startAlpha_, endAlpha_, SoLib::easeInQuad(animTimer_.GetProgress()));
		isChanging_ = true;
	}
	else {
		isChanging_ = false;
		// 次の行動リクエストに待機アニメーションを設定
		behaviorRequest_ = kStay;
		// 非表示中
		isDisplay_ = false;

		// 次の進捗と一致しない場合はそれを代入する
		if (nextProgress_ != progress_) {
			progress_ = nextProgress_;
		}
	}
}
