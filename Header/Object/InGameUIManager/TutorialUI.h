#pragma once
#include "../../../Utils/SoLib/SoLib.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"

/// <summary>
/// チュートリアル用UI
/// </summary>
class TutorialUI
{
public: // サブクラス　

	// このUI用の行動列挙子
	enum TutorialUIBehavior {
		kStay,
		kAppear,
		kDisappear
	};

	// チュートリアルの進行度列挙子
	enum TutorialProgress {
		kMove,
		kEnableSpin,
		kSpin,
		kDisableSpin,
		kGoal
	};
	
public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="id">このスプライトの名前</param>
	void Init(const std::string& id);

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw();

	// ImGuiの描画
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// 次のUIアニメーションセッター
	/// </summary>
	/// <param name="b">次のUIアニメーションで何を行うか</param>
	void SetBehavior(TutorialUIBehavior b) { behaviorRequest_ = b; }

public: // アニメーション関数

	/// <summary>
	/// 待機状態初期化
	/// </summary>
	void StayInit();
	/// <summary>
	/// 待機状態の更新
	/// </summary>
	void StayUpdate();

	/// <summary>
	/// 表示状態初期化
	/// </summary>
	void AppearInit();
	/// <summary>
	/// 表示状態更新
	/// </summary>
	void AppearUpdate();

	/// <summary>
	/// 消滅状態初期化
	/// </summary>
	void DisappearInit();
	/// <summary>
	/// 消滅状態更新
	/// </summary>
	void DisappearUpdate();

public: // パブリックなメンバ変数

	// 位置座標
	Vector2 position_ = { 0.0f, 0.0f };
	// 大きさ
	Vector2 scale_ = { 512.0f, 256.0f };
	// 中心点
	Vector2 anchorPoint_ = { 0.5f, 0.5f };

	// スプライトの透明度
	float overrapSpriteAlpha_ = 1.0f;

private: // メンバ変数

	// それぞれの演出時間
	const float kAppearTime_ = 1.0f; // 表示
	const float kDisappearTime_ = 0.8f; // 非表示

	// ImGui表示名
	std::string id_;

	// ふるまい
	TutorialUIBehavior behavior_ = TutorialUIBehavior::kStay;
	// ふるまいリクエスト
	std::optional<TutorialUIBehavior> behaviorRequest_ = std::nullopt;

	// 演出用タイマー
	SoLib::DeltaTimer animTimer_;

	// 線形補間用パラメーター保存用
	Vector2 startTranslate_ = { 0.0f, 0.0f }; // 大きさ始端値
	Vector2 endTranslate_ = { 0.0f, 0.0f };   // 大きさ終端値

	// スプライト本体
	std::unique_ptr<Sprite>tutorialUI_; // コントローラー
	std::unique_ptr<Sprite>tutorialKeyUI_; // キーボード用

	// ImGuiでUI行動状態を選択するための変数
	int imGuiSelectBehavior_;

};

