#pragma once
#include <string>
#include <optional>
#include "../../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../../Utils/SoLib/SoLib.h"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../../../Engine/DirectBase/Input/Input.h"

/// <summary>
/// ポーズ画面のマネージャー
/// </summary>
class PoseManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	PoseManager() = default;
	~PoseManager() = default;
	PoseManager(const PoseManager&) = delete;
	const PoseManager& operator=(const PoseManager&) = delete;

private: // サブクラス

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

public: // パブリックなサブクラス

	// ポーズ画面の状態列挙子
	enum PoseState {
		kResume,
		kReturnCheckPoint,
		kRetry,
		kReturnStageSelect,
		kNone = 999,
	};

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static PoseManager* GetInstance() {
		static PoseManager instance;
		return &instance;
	};

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
	/// スプライト描画関数
	/// </summary>
	/// <param name="camera">使用するカメラ</param>
	void Draw();

public: // その他関数

	/// <summary>
	/// ポーズメニューを展開する関数
	/// </summary>
	void DeployPoseMenu();

	/// <summary>
	/// 現在実行中のカテゴリゲッター
	/// </summary>
	/// <returns>現在実行中のカテゴリ</returns>
	PoseState GetPoseState() { return executioningCategory_; }

	/// <summary>
	/// ポーズ有効状態のゲッター
	/// </summary>
	/// <returns>ポーズが有効かどうか</returns>
	bool GetIsActive() { return isActive_; }

private	: // プライベートなメンバ関数

	/// <summary>
	/// ポーズメニューを閉じる関数
	/// </summary>
	void CloseMenu();

	/// <summary>
	/// 決定ボタンを押したときの関数
	/// </summary>
	void PressAButton();

private: // メンバ変数

	// 入力検知
	Input* input_ = nullptr;

	// ポーズ画面
	UISprite poseUI_;

	Vector2 poseUIStartPos_;
	Vector2 poseUIEndPos_;
	Vector4 poseUIStartColor_;
	Vector4 poseUIEndColor_;

	// 選択中のカテゴリ
	int selectedCategory_ = 0;

	// 現在実行中のカテゴリ
	PoseState executioningCategory_ = kNone;

	// ポーズ状態か
	bool isActive_ = false;
	//表示中か
	bool isDisplay_ = false;

	// ポーズメニューを閉じるボタン
	bool close_ = false;

	// ポーズUIの展開アニメーションタイマー
	SoLib::DeltaTimer timer_;
};