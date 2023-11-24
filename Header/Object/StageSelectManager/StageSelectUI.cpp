#include "StageSelectUI.h"
#include "StageSelectManager.h"

void StageSelectUI::Init()
{
	// 選択マネージャーのインスタンス取得
	stageSelectManager_ = StageSelectManager::GetInstance();

	// ステージ選択画面
	stageSelectUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/StageSelect/StageSelectUI.png")));
	stageSelectUI_.scale_ = { 1280.0f, 720.0f };
	
	// ステージプレビュー画像用UI
	stagePreviewUI_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/StageSelect/Previews/Preview1.png")));
	stagePreviewUI_.position_ = { 1280.0f / 2.0f, 380.0f };
	stagePreviewUI_.scale_ = { 720.0f, 480.0f };
	stagePreviewUI_.anchorPoint_ = { 0.5f, 0.5f };

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
	if (prevSelectedStageNumber_ != stageSelectManager_->GetSelectedStageNumber()) {
		// テクスチャ名の取得
		std::string textureName = "UI/TD2_3week_2/StageSelect/Previews/Preview" + std::to_string(stageSelectManager_->GetSelectedStageNumber() + 1) + ".png";
		stagePreviewUI_.sprite_->SetTextureHaundle(TextureManager::Load(textureName));

		// 選択ステージ番号の取得
		prevSelectedStageNumber_ = stageSelectManager_->GetSelectedStageNumber();
	}
}

void StageSelectUI::Draw()
{
	// 各UIスプライトの描画
	stageSelectUI_.Draw(); // ステージ選択画面
	stagePreviewUI_.Draw(); // ステージプレビュー
	leftArrowUI_.Draw();   // ステージ選択画面左矢印
	rightArrowUI_.Draw();  // ステージ選択画面右矢印
}

void StageSelectUI::DisplayImGui()
{
	ImGui::Begin("StageSelectUI");
	stageSelectUI_.DisplayImGui("StageSelectUI");
	stagePreviewUI_.DisplayImGui("StagePreviewUI");
	leftArrowUI_.DisplayImGui("LeftArrowUI");
	rightArrowUI_.DisplayImGui("RightArrowUI");
	ImGui::End();
}
