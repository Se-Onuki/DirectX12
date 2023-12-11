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

	
	void ChangeTexture();

	/// <summary>
	/// 引数で指定したチュートリアル画像を表示する関数
	/// </summary>
	/// <param name="progress"></param>
	void DisplayNextTutorial(int progress);

	/// <summary>
	/// 次のUIアニメーションセッター
	/// </summary>
	/// <param name="b">次のUIアニメーションで何を行うか</param>
	void SetBehavior(TutorialUIBehavior b) { behaviorRequest_ = b; }

	/// <summary>
	/// 次のチュートリアルテクスチャセッター
	/// </summary>
	/// <param name="progress">チュートリアル段階</param>
	void SetTutorialTexture(TutorialProgress progress) { progress_ = progress; }

	/// <summary>
	/// 入力にコントローラーが使用されているかセッター
	/// </summary>
	/// <param name="is">コントローラーかどうか</param>
	void SetIsController(bool is) { isController_ = is; }

	/// <summary>
	/// アニメーションが行われているかゲッター
	/// </summary>
	/// <returns>アニメーションが行われているか</returns>
	bool GetIsChanging() { return isChanging_; }

	/// <summary>
	/// 表示状態ゲッター
	/// </summary>
	/// <returns>表示状態</returns>
	bool GetIsDisplay() { return isDisplay_; }

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
	Vector2 position_ = { 640.0f, -80.0f };
	// 大きさ
	Vector2 scale_ = { 341.3f, 170.6f };
	// 中心点
	Vector2 anchorPoint_ = { 0.5f, 0.5f };

	// スプライトの透明度
	float alpha_ = 0.0f;
	float alphaMagnification_ = 1.0f;

private: // メンバ変数

	// それぞれの演出時間
	const float kAppearTime_ = 0.8f; // 表示
	const float kDisappearTime_ = 0.6f; // 非表示

	// ImGui表示名
	std::string id_;

	// ふるまい
	TutorialUIBehavior behavior_ = TutorialUIBehavior::kStay;
	// ふるまいリクエスト
	std::optional<TutorialUIBehavior> behaviorRequest_ = std::nullopt;

	// 演出用タイマー
	SoLib::DeltaTimer animTimer_;

	// 線形補間用パラメーター保存用
	Vector2 startTranslate_ = { 640.0f, -80.0f }; // 大きさ始端値
	Vector2 endTranslate_ = { 640.0f, -80.0f };   // 大きさ終端値
	float startAlpha_ = 1.0f; // 大きさ始端値
	float endAlpha_ = 0.0f;   // 大きさ終端値

	// スプライト本体
	std::unique_ptr<Sprite>tutorialUI_; // コントローラー
	std::unique_ptr<Sprite>tutorialKeyUI_; // キーボード用

	// チュートリアル進捗
	int progress_ = kMove; // 現在の
	int prevProgress_ = kDisableSpin; // 前の

	// コントローラーか
	bool isController_ = false;
	// 表示中か
	bool isDisplay_ = false;

	// アニメーション中か
	bool isChanging_ = false;

	// チュートリアル変更中か
	bool changeTutorial_ = false;

	// ImGuiでUI行動状態を選択するための変数
	int imGuiSelectBehavior_;

};

