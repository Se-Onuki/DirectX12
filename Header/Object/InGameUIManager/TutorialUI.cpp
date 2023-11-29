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
}

void TutorialUI::Draw()
{
}

void TutorialUI::DisplayImGui()
{
}

void TutorialUI::StayInit()
{
}

void TutorialUI::StayUpdate()
{
}

void TutorialUI::AppearInit()
{
}

void TutorialUI::AppearUpdate()
{
}

void TutorialUI::DisappearInit()
{
}

void TutorialUI::DisappearUpdate()
{
}
