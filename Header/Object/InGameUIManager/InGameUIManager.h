#pragma once
#include <list>
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../Entity/Component/PlayerComp.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "StarUI.h"
#include "TutorialUI.h"

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

	/// <summary>
	/// 引数で指定した数の星を追加
	/// </summary>
	void SetStar(int count);

	/// <summary>
	/// 星のスタックがない場合、最初星を振動させる関数
	/// </summary>
	void ShakeStar();

private: // プライベートなメンバ関数

	/// <summary>
	/// 振動中呼び出される関数
	/// </summary>
	void ShakingStar();

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

	// チュートリアルＵＩ
	std::unique_ptr<TutorialUI> tutorialUI_;
	// チュートリアル進捗
	int tutorialProgress_ = TutorialUI::TutorialProgress::kMove;

	// 振動範囲
	const float kShakeRange = 10.0f;
	const float kShakeTime = 0.5f;

	// 3ステージ目で出現するリプレイ方法を示すスプライト
	UISprite replayTutorialUI_;
	// 上スプライトのアニメーション用タイマー
	SoLib::DeltaTimer replaySpriteAnimTimer_;
	// リプレイ方法を示すスプライトの回数変数
	int replayAnimCount_ = 0;

	// 星UIの振動トリガー
	bool blurTrigger_ = false;
	// 振動用方向ベクトル
	Vector2 shakeVector_ = { 0.0f, 0.0f };
	// 振動範囲設定
	float shakeRange_ = kShakeRange;
	// 振動演出用タイマー
	SoLib::DeltaTimer shakeAnimTimer_;

	// 星UIの量を示すUIを加算するか減算するか
	int imGuiAddType_ = 0;

	static uint32_t dontSpinSE_;

};

