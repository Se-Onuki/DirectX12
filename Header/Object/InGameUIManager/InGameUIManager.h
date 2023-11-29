#pragma once
#include <list>
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../Entity/Component/PlayerComp.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "StarUI.h"

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
	/// <param name="maxStarCount">最大星数</param>
	void Init(int maxStarCount);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime);

	/// <summary>
	/// スプライト描画関数
	/// </summary>
	void Draw();

public: // アクセッサ等
	
	/// <summary>
	/// プレイヤーコンポーネントのセッター
	/// </summary>
	/// <param name="pComp">プレイヤーコンポーネント実体</param>
	void SetPlayerComp(const PlayerComp& pComp) { playerComp_ = &pComp; }

	/// <summary>
	/// 星を追加する関数
	/// </summary>
	/// <param name="p0m">プラスかマイナスか</param>
	void AddStar(int p0m);

public: // パブリックなメンバ変数

	// ゲーム画面全体の透明度
	float uiAlpha_ = 1.0f;

private: // メンバ変数

	// プレイヤーコンポーネント
	const PlayerComp* playerComp_ = nullptr;

	// ステージ選択マネージャ
	int selectedStageNumber_ = 0;

	// 最大星数
	int maxStarCount_;

	// 星のUIスプライト
	std::list<std::unique_ptr<StarUI>> stars_;
	// 星のUIスプライトの始点
	Vector2 starUIsStartingPoint_ = { 900.0f, 64.0f };
	// 星UIの行間設定
	float starUILineSpace_ = 150.0f;

	// 操作方法スプライト
	UISprite controllUI_;
	UISprite spinControllUI_;

	// スピンモード中か
	bool isSpining_ = false;

	// 星UIの量を示すUIを加算するか減算するか
	int imGuiAddType_ = 0;

};

