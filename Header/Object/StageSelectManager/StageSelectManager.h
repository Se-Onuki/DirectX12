#pragma once
#include "../../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include "StageSelectUI.h"

/// <summary>
/// ステージ選択マネージャ
/// </summary>
class StageSelectManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	StageSelectManager() = default;
	~StageSelectManager() = default;
	StageSelectManager(const StageSelectManager&) = delete;
	const StageSelectManager& operator=(const StageSelectManager&) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static StageSelectManager* GetInstance() {
		static StageSelectManager instance;
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
	/// 3Dモデル描画関数
	/// </summary>
	/// <param name="camera">使用するカメラ</param>
	void Draw(const Camera3D& camera);

	/// <summary>
	/// スプライト用描画関数
	/// </summary>
	void SpriteDraw();

	/// <summary>
	/// ImGuiの表示関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// 選択中のステージ番号ゲッター
	/// </summary>
	/// <returns>選択中ステージ番号</returns>
	int GetSelectedStageNumber() { return selectedStageNumber_; }

	/// <summary>
	/// 調整項目クラスに値を追加する関数
	/// </summary>
	void AddItem();

	/// <summary>
	/// 調整項目クラスから値を取得する関数
	/// </summary>
	void ApplyItem();

private: // メンバ変数

	// 調整項目クラス
	GlobalVariables* gv_ = nullptr;

	// 入力検知用
	Input* input_ = nullptr;

	// 選択中のステージ番号
	int selectedStageNumber_ = 0;
	// ステージの最大数
	int maxLevelCount_ = 10;

	// UI関係
	StageSelectUI ui_;

	// 効果音系
	static uint32_t selectStageSE_; // ステージ選択SE

};

