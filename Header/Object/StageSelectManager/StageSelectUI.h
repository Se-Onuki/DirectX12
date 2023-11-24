#pragma once
#include <string>
#include <optional>
#include "../../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../../Utils/SoLib/SoLib.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"

// クラスの前方宣言
class StageSelectManager;

/// <summary>
/// ステージ選択画面のUIクラス
/// </summary>
class StageSelectUI
{
private:// サブクラス

	struct UISprite {
		Vector2 position_ = { 0.0f, 0.0f };
		Vector2 scale_ = { 1.0f, 1.0f };
		Vector2 anchorPoint_ = { 0.0f, 0.0f };
		std::unique_ptr<Sprite>sprite_;

		// 描画
		void Draw() {
			sprite_->SetPosition(position_);
			sprite_->SetScale(scale_);
			sprite_->SetPivot(anchorPoint_);
			sprite_->Draw();
		}

		// ImGuiの描画
		void DisplayImGui(std::string id){
			if (ImGui::TreeNode(id.c_str())) {
				ImGui::DragFloat2("Position", &position_.x, 1.0f);
				ImGui::DragFloat2("Scale", &scale_.x, 1.0f);
				ImGui::DragFloat2("AnchorPoint", &anchorPoint_.x, 0.01f);
				ImGui::TreePop();
			}
		}
	};
	
	/// <summary>
	/// UI行動列挙子
	/// </summary>
	enum Behavior {
		kRoot,
		KChangeStage,
	};

public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// アニメーション再生中かのゲッター
	/// </summary>
	/// <returns>アニメーション再生中か</returns>
	bool GetIsPlayingAnim() { return isPlayingAnim_; }

	/// <summary>
	/// 左右どちらのボタンを押したかのセッター
	/// </summary>
	/// <param name="isRight">右か左か</param>
	void SetIsRight(bool isRight) { isRight_ = isRight; }

public: // アニメーション関数

	/// <summary>
	/// 通常アニメーション初期化
	/// </summary>
	void RootInitialize();

	/// <summary>
	/// 通常
	/// </summary>
	void RootUpdate();

	/// <summary>
	///	選択ステージ変更アニメーション初期化
	/// </summary>
	/// <param name="isRight">右</param>
	void ChangeInitialize(bool isRight);

	/// <summary>
	/// 選択ステージ変更アニメーション初期化
	/// </summary>
	void ChangeUpdate();

private: // メンバ変数

	// ステージ選択マネージャー
	StageSelectManager* stageSelectManager_ = nullptr;

	// 前フレーム選択したステージ番号
	int prevSelectedStageNumber_;

	/// スプライト関係
	UISprite stageSelectUI_; // ステージ選択画面
	UISprite stagePreviewUI_; // ステージプレビュー
	UISprite stagePreviewUI2_; // ステージプレビュー(アニメーション用)
	UISprite leftArrowUI_;   // ステージ選択画面左矢印
	UISprite rightArrowUI_;  // ステージ選択画面右矢印

	// ふるまい
	Behavior behavior_ = Behavior::kRoot;
	// ふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 選択ステージアニメーション切り替えトリガー
	bool isRight_ = false;

	// プレビュー画像アニメーション用の座標保存用変数
	Vector2 startPreviewPos_; // 始端値
	Vector2 endPreviewPos_;	  // 終端値
	Vector2 startPreviewPos2_; // 始端値(二つ目用)
	Vector2 endPreviewPos2_;   // 終端値(二つ目用)

	// 選択矢印アニメーション用の座標保存用変数
	Vector2 startSelectArrowPos_L_; // 始端値（左）
	Vector2 startSelectArrowPos_R_; // 始端値（右）
	Vector2 endSelectArrowPos_L_; // 終端値（左）
	Vector2 endSelectArrowPos_R_; // 終端値（右）
	// 選択矢印トリガー
	bool selectArrowAnim_;

	// 通常アニメーション秒数
	float rootAnimTime_ = 2.0f;
	float stageChangeAnimTime = 1.0f;

	// 演出トリガー
	bool isPlayingAnim_ = false;

	// ステージプレビューのアニメーションタイマー
	SoLib::DeltaTimer prevAnimTimer_;
	// 選択矢印のアニメーションタイマー
	SoLib::DeltaTimer selectArrowTimer_;

};
