#include "StageSelectUI.h"

void StageSelectUI::Init()
{
	// ステージ選択画面
	stageSelectUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectUI.png")));
	stageSelectUI_.scale_ = { 1280.0f, 720.0f };
	
	// 左矢印UI
	leftArrowUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectCursorLeft.png")));
	leftArrowUI_.position_ = { 35.0f, 400.0f };
	leftArrowUI_.scale_ = { 159.0f, 145.0f };
	leftArrowUI_.anchorPoint_ = { 0.0f, 0.5f };
	
	// 右矢印UI
	rightArrowUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectCursorRight.png")));
	rightArrowUI_.position_ = { 1086.0f, 400.0f };
	rightArrowUI_.scale_ = { 159.0f, 145.0f };
	rightArrowUI_.anchorPoint_ = { 0.0f, 0.5f };
}

void StageSelectUI::Update([[maybe_unused]] float deltaTime)
{
		
}

void StageSelectUI::Draw()
{
	// 各UIスプライトの描画
	stageSelectUI_.Draw(); // ステージ選択画面
	leftArrowUI_.Draw();   // ステージ選択画面左矢印
	rightArrowUI_.Draw();  // ステージ選択画面
}

void StageSelectUI::DisplayImGui()
{
	ImGui::Begin("StageSelectUI");
	stageSelectUI_.DisplayImGui("StageSelectUI");
	leftArrowUI_.DisplayImGui("LeftArrowUI");
	rightArrowUI_.DisplayImGui("RightArrowUI");
	ImGui::End();
}
