#pragma once
#include"../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../Entity/Entity.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"

/// <summary>
/// タイトル画面のマネージャー
/// </summary>
class TitleManager
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
		void DisplayImGui(std::string id) {
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
	void Initialize();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 3Dモデル描画関数
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera3D& camera);

	/// <summary>
	/// スプライト描画関数
	/// </summary>
	void SpriteDraw();

private: // メンバ変数

	// 入力検知用
	Input* input_ = nullptr;

	// カメラマネージャー
	CameraManager* cameraManager_ = nullptr;

	// 天球
	std::unique_ptr<GameObject> skyDome_;

	// プレイヤー
	std::unique_ptr<GameObject> playerModel_;
	// ブロック
	std::unique_ptr<GameObject> blockModel_;

	UISprite titleUI_;
	UISprite circleUI_;

	UISprite buttonUI_;

	float rotateImage_;

	// プレビュー画像アニメーション用の座標保存用変数
	Vector2 startTitlePos_; // 始端値
	Vector2 endTitlePos_;	  // 終端値
	Vector2 startCirclePos_; // 始端値(二つ目用)
	Vector2 endCirclePos_;   // 終端値(二つ目用)
	Vector2 startButtonPos_; // 始端値
	Vector2 endButtonPos_;	  // 終端値

	// アニメーション用タイマー
	SoLib::DeltaTimer timer_;

	// カメラアニメーション切り替えトリガー
	bool isReturn_ = false;

};

