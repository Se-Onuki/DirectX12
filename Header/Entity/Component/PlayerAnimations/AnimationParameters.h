#pragma once
#include "../../../../externals/imgui/imgui.h"
#include"../../../../Engine/DirectBase/File/VariantItem.h"
#include "../../../../Utils/Math/Vector3.h"
#include "AnimationKeys.h"

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

private: // メンバ変数

	GlobalVariables* globalVariables_ = nullptr;

	// アイドルアニメーションのキー
	AnimationKeys idleAnimKeys_;

	// 移動アニメーションキー
	AnimationKeys moveAnimKeys_;

	// ジャンプアニメーション開始キー
	AnimationKeys startJumpAnimKeys_;
	// 滞空アニメーションキー
	AnimationKeys hoveringAnimKeys_;
	// 着地アニメーションキー
	AnimationKeys landAnimKeys_;

	// 回転開始アニメーションキー
	AnimationKeys startRotateAnimKeys_;
	// 回転中アニメーションキー
	AnimationKeys rotatingAnimKeys_;
	// 回転終了アニメーションキー
	AnimationKeys rotateEndAnimationKeys_;

	// クリアアニメーションキー
	AnimationKeys clearAnimationKeys_;
	// クリアアニメーション後の待機キー
	AnimationKeys clearBleakAnimationKeys_;

	// タイトルアニメーションキー
	AnimationKeys titleAnimationKeys_;

};

