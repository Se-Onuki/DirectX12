#include "InGameUIManager.h"

void InGameUIManager::Init(int maxStarCount)
{
	// 最大星数を取得
	maxStarCount_ = maxStarCount;

	// 最大星数分スプライトを追加
	for (int i = 0; i < maxStarCount_; i++) {
		// 新しいUIを生成
		std::unique_ptr<StarUI> newUI = std::make_unique<StarUI>();
		// 生成したUIを配列に追加
		std::string addName = "Star" + std::to_string(i);
		newUI->Init(addName);
		stars_.push_back(std::move(newUI));
	}
}

void InGameUIManager::Update(float deltaTime)
{
	// 最大星数分ループ
	int count = 0;
	for (auto& star : stars_) {
		star->Update(deltaTime);
		star->position_ = starUIsStartingPoint_;
		star->position_.x = starUIsStartingPoint_.x + (starUILineSpace_ * count);
		count++;
	}

#ifdef _DEBUG
	ImGui::Begin("InGameUI");
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

	ImGui::End();
#endif // _DEBUG

}

void InGameUIManager::Draw()
{
	// 最大星数分ループ
	for (auto& star : stars_) {
		star->Draw();
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
