#pragma once
#include "../../../../externals/imgui/imgui.h"
#include"../../../../Engine/DirectBase/File/VariantItem.h"
#include "../../../../Utils/Math/Vector3.h"
#include "PlayerBone.h"

// クラスの前方宣言
class GlobalVariables;

/// <summary>
/// アニメーション関連の全てを管理するクラス
/// </summary>
class AnimationParameters
{
public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update();

	/// <summary>
	/// グローバル変数クラスに値を追加
	/// </summary>
	void AddItem();

	/// <summary>
	/// グローバル変数クラスに値を適用
	/// </summary>
	void SetItem();

	/// <summary>
	/// グローバル変数クラスから値を取得
	/// </summary>
	void ApplyItem();

private: // メンバ変数

	GlobalVariables* globalVariables_ = nullptr;

#pragma region Idle
	// アイドル状態のアニメーション時間
	float idleAnimationTime_ = 1.0f;

	// 
	PlayerBone startIdle_;
	PlayerBone endIdle_;

#pragma endregion

#pragma region Move
	// 移動アニメーション時間
	float moveAnimationTime_ = 1.0f;

	// 
	PlayerBone startMove_;
	PlayerBone endMove_;

#pragma endregion

};

