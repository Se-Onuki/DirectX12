#include "StarUI.h"
#include "../../../Engine/DirectBase/Base/TextureManager.h"

void StarUI::Init(const std::string& id)
{
	// ID取得
	id_ = id;
	// スプライトのインスタンスを生成
	starBackGroundUI_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/SpinStarNoStuck.png")));
	starUI_.reset(Sprite::Create(TextureManager::Load("UI/TD2_3week_2/InGame/SpinStarFillStuck.png")));

}

void StarUI::Update(float deltaTime)
{

	// ふるまいを変更するリクエストがあればTrue
	if (behaviorRequest_) {
		// ふるまい変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case StarUIBehavior::kStay:
			// 待機アニメーション初期化
			StayInit();
			break;
		case StarUIBehavior::kAppear:
			// 表示アニメーション初期化
			AppearInit();
			break;
		case StarUIBehavior::kDisappear:
			// 非表示アニメーション初期化
			DisappearInit();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	// 各振る舞いごとの更新を実行
	switch (behavior_) {
	case StarUIBehavior::kStay:
		// 待機アニメーション初期化
		StayUpdate();
		break;
	case StarUIBehavior::kAppear:
		// 行うアニメーション自体は同一なので同じ関数を使う
		AppearUpdate();
		break;
	case StarUIBehavior::kDisappear:
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

void StarUI::Draw()
{
	// 背景の描画
	starBackGroundUI_->SetPosition(position_);
	starBackGroundUI_->SetScale(scale_);
	starBackGroundUI_->SetPivot(anchorPoint_);
	starBackGroundUI_->Draw();

	// スプライトの描画
	starUI_->SetPosition(position_);
	starUI_->SetScale(overrapSpriteScale_);
	starUI_->SetPivot(anchorPoint_);
	starUI_->SetColor({ 1.0f, 1.0f, 1.0f, overrapSpriteAlpha_ });
	starUI_->Draw();
}

void StarUI::DisplayImGui()
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

		if (ImGui::Button("Change Behavior")) {
			behaviorRequest_ = (StarUIBehavior)imGuiSelectBehavior_;
		}

		ImGui::TreePop();
	}
}

void StarUI::StayInit()
{
	// 設定された終端値に瞬時に合わせる
	overrapSpriteScale_ = endScale_;
	overrapSpriteAlpha_ = 1.0f;

	// タイマーを強制停止
	animTimer_.Clear();
}

void StarUI::StayUpdate()
{
	// 更新処理は今のところ記述なし
}

void StarUI::AppearInit()
{
	// 現段階のメンバ変数のパラメーターを取得
	startScale_ = overrapSpriteScale_; // 大きさ始端値
	endScale_ = scale_;				   // 大きさ終端値
	overrapSpriteAlpha_ = 1.0f;

	// この星は溜まっている
	isFilled_ = true;

	// タイマーを開始
	animTimer_.Start(kAppearTime_);
}

void StarUI::AppearUpdate()
{
	if (not animTimer_.IsFinish()) {
		// パラメーターを線形補間で変更する
		overrapSpriteScale_ = SoLib::Lerp(startScale_, endScale_, SoLib::easeOutBack(SoLib::easeOutQuad(animTimer_.GetProgress())));
		overrapSpriteAlpha_ = 1.0f;
	}
	else {
		// 次の行動リクエストに待機アニメーションを設定
		behaviorRequest_ = kStay;
	}
}

void StarUI::DisappearInit()
{
	// 現段階のメンバ変数のパラメーターを取得
	startScale_ = overrapSpriteScale_; // 大きさ始端値
	endScale_ = { 0.0f, 0.0f };		   // 大きさ終端値

	// この星は溜まっている
	isFilled_ = false;

	// タイマーを開始
	animTimer_.Start(kDisappearTime_);
}

void StarUI::DisappearUpdate()
{
	if (not animTimer_.IsFinish()) {
		// パラメーターを線形補間で変更する
		overrapSpriteScale_ = SoLib::Lerp(startScale_, endScale_, SoLib::easeInBack(SoLib::easeInQuad(animTimer_.GetProgress())));
	}
	else {
		// 次の行動リクエストに待機アニメーションを設定
		behaviorRequest_ = kStay;
	}
}
