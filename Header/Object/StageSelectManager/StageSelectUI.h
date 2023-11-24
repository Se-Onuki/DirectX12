#pragma once
#include <string>
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

public: // アニメーション関数

	/// <summary>
	/// UIの通常アニメーションの初期化
	/// </summary>
	void InitializeIdle();

	/// <summary>
	/// 通常
	/// </summary>
	void IdleUpdate();

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

	// ステージプレビューのアニメーションタイマー
	SoLib::DeltaTimer prevAnimTimer_;

};

