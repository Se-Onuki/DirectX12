#pragma once
#include <vector>
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"

/// <summary>
/// ゲーム中のUIマネージャ
/// </summary>
class InGameUIManager
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
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime);

	/// <summary>
	/// スプライト描画関数
	/// </summary>
	void Draw();

private: // メンバ変数

	// 回転可能☆数のスプライト
	std::vector<UISprite> starSprite_;

};

