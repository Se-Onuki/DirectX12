#pragma once
#include "BaseAnimation.h"

/// <summary>
/// 移動アニメーションクラス
/// </summary>
class Move : public BaseAnimation
{
public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize(float transitionTime = 0.0f)override;

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime)override;

private: // メンバ変数

	// リターントリガー
	bool isReturn_;

};

