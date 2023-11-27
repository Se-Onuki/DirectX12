#pragma once
#include "../../../../Utils/SoLib/SoLib.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../Entity.h"

/// <summary>
/// ゴール演出用コンポーネント
/// </summary>
class GoalAnimComp : public IComponent
{
public: // メンバ関数

	// 基底コンポーネントのコンストラクタを使用
	using IComponent::IComponent;
	// デストラクタ
	~GoalAnimComp() = default;

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update() override;

	/// <summary>
	/// ImGui表示関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// プレイヤーモデルのセッター
	/// </summary>
	/// <param name="entity">プレイヤーアニメーションを持つエンティティ</param>
	void SetPlayerModel(Entity* entity) { playerModel_ = entity; }

public: // その他関数群

	/// <summary>
	/// ゴールアニメーション再生関数
	/// </summary>
	void PlayGoalAnim();

private: // メンバ変数

	// カメラマネージャーのインスタンス
	CameraManager* cameraManager_ = nullptr;

	// ゴール時のカメラ
	Camera3D* goalCamera_ = nullptr;
	// ゴール時のカメラのオフセット
	Vector3 offsetTranslate_ = Vector3::zero; // 位置
	Vector3 offsetRotate_ = Vector3::zero;	  // 回転

	// プレイヤーモデル
	Entity* playerModel_ = nullptr;

	//　再生
	bool isPlay_ = false;
	
	// 終了トリガー
	bool isEnd_ = false;
};

