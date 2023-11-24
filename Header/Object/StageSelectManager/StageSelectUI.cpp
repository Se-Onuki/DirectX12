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

	stagePreviewUI2_.sprite_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/StageSelect/Previews/Preview1.png")));
	stagePreviewUI2_.position_ = { -1280.0f / 2.0f, 380.0f };
	stagePreviewUI2_.scale_ = { 720.0f, 480.0f };
	stagePreviewUI2_.anchorPoint_ = { 0.5f, 0.5f };

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

	// 演出中ではない
	isPlayingAnim_ = false;

	// 通常アニメーションで初期化
	RootInitialize();
}

void StageSelectUI::Update([[maybe_unused]] float deltaTime)
{
	if (prevSelectedStageNumber_ != stageSelectManager_->GetSelectedStageNumber()) {
		// ステージ変更
		behaviorRequest_ = KChangeStage;

		// 選択ステージ番号の取得
		prevSelectedStageNumber_ = stageSelectManager_->GetSelectedStageNumber();
	}

	// ふるまいを変更するリクエストがあればTrue
	if (behaviorRequest_) {
		// ふるまい変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			// 通常アニメーション初期化
			RootInitialize();
			break;
		case Behavior::KChangeStage:
			// ステージ選択アニメーション初期化
			ChangeInitialize(isRight_);
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	// 各振る舞いごとの更新を実行
	switch (behavior_) {
	case Behavior::kRoot:
		// 通常アニメーション更新
		RootUpdate();
		break;
	case Behavior::KChangeStage:
		// ステージ選択アニメーション更新
		ChangeUpdate();
		break;
	}

	// 演出タイマーの更新
	prevAnimTimer_.Update(deltaTime);
}

void StageSelectUI::Draw()
{
	// 各UIスプライトの描画
	stageSelectUI_.Draw(); // ステージ選択画面
	stagePreviewUI_.Draw(); // ステージプレビュー
	stagePreviewUI2_.Draw(); // ステージプレビュー(アニメーション用)
	leftArrowUI_.Draw();   // ステージ選択画面左矢印
	rightArrowUI_.Draw();  // ステージ選択画面右矢印
}

void StageSelectUI::DisplayImGui()
{
	ImGui::Begin("StageSelectUI");
	stageSelectUI_.DisplayImGui("StageSelectUI");
	stagePreviewUI_.DisplayImGui("StagePreviewUI");
	stagePreviewUI2_.DisplayImGui("StagePreviewUI2");
	leftArrowUI_.DisplayImGui("LeftArrowUI");
	rightArrowUI_.DisplayImGui("RightArrowUI");
	ImGui::End();
}

void StageSelectUI::RootInitialize()
{
	// 画像アニメーション用変数の初期化
	startPreviewPos_ = { 1280.0f / 2.0f, 380.0f };
	endPreviewPos_ = { 1280.0f / 2.0f, 390.0f };

	// タイマー開始
	prevAnimTimer_.Start(rootAnimTime_);
}

void StageSelectUI::RootUpdate()
{
	// 
	if (not prevAnimTimer_.IsFinish()) {
		stagePreviewUI_.position_ = SoLib::Lerp(startPreviewPos_, endPreviewPos_, SoLib::easeOutQuad(prevAnimTimer_.GetProgress()));
	}
	else {
		stagePreviewUI_.position_ = endPreviewPos_;
		endPreviewPos_ = startPreviewPos_;
		startPreviewPos_ = stagePreviewUI_.position_;
		prevAnimTimer_.Start(rootAnimTime_);
	}
}

void StageSelectUI::ChangeInitialize(bool isRight)
{
	// 開始地点を設定
	startPreviewPos_ = stagePreviewUI_.position_;
	// 終端地点を設定
	endPreviewPos2_ = { 1280.0f / 2.0f, 380.0f };

	// テクスチャ名の取得
	std::string textureName = "UI/TD2_3week_2/StageSelect/Previews/Preview" + std::to_string(stageSelectManager_->GetSelectedStageNumber() + 1) + ".png";
	stagePreviewUI2_.sprite_->SetTextureHaundle(TextureManager::Load(textureName));

	// 右か左か
	if (isRight) {
		startPreviewPos2_ = { (-1280.0f * 2.0f) + (1280.0f / 2.0f), 380.0f};
		endPreviewPos_ = { (1280.0f * 2.0f) - (1280.0f / 2.0f), 380.0f };
	}
	else {
		startPreviewPos2_ = { (1280.0f * 2.0f) - (1280.0f / 2.0f), 380.0f };
		endPreviewPos_ = { (-1280.0f * 2.0f) + (1280.0f / 2.0f), 380.0f };
	
	}

	// 演出中に
	isPlayingAnim_ = true;
	prevAnimTimer_.Start(stageChangeAnimTime);
}

void StageSelectUI::ChangeUpdate()
{
	// 
	if (not prevAnimTimer_.IsFinish()) {
		stagePreviewUI_.position_ = SoLib::Lerp(startPreviewPos_, endPreviewPos_, SoLib::easeInQuad(prevAnimTimer_.GetProgress()));
		stagePreviewUI2_.position_ = SoLib::Lerp(startPreviewPos2_, endPreviewPos2_, SoLib::easeOutQuad(prevAnimTimer_.GetProgress()));
	}
	else {
		// テクスチャ名の取得
		std::string textureName = "UI/TD2_3week_2/StageSelect/Previews/Preview" + std::to_string(stageSelectManager_->GetSelectedStageNumber() + 1) + ".png";
		stagePreviewUI_.sprite_->SetTextureHaundle(TextureManager::Load(textureName));

		stagePreviewUI_.position_ = { 1280.0f / 2.0f, 380.0f };
		stagePreviewUI2_.position_ = { -1280.0f / 2.0f, 380.0f };

		behaviorRequest_ = kRoot;

		// 演出終了に
		isPlayingAnim_ = false;
	}
}
