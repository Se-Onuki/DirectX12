#include "InGameUIManager.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../../Header/Object/StageSelectManager/StageSelectManager.h"
#include "../LevelElementManager.h"

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
		star->position_ = starUIsStartingPoint_;
		star->position_.x = starUIsStartingPoint_.x + (starUILineSpace_ * count);
		star->overrapSpriteAlpha_ = uiAlpha_;
		count++;
	}

	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_->Update(deltaTime);
		tutorialUI_->alphaMagnification_ = uiAlpha_;
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

	controllUI_.sprite_->SetColor({ 1.0f, 1.0f, 1.0f, uiAlpha_ });

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

	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_->DisplayImGui();
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

	// 選択されたステージ番号が0だった時
	if (selectedStageNumber_ == 0) {
		// インスタンス生成
		tutorialUI_->Draw();
	}
}

void InGameUIManager::AddStar(int p0m)
{
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
	blurTrigger_ = true;
}
