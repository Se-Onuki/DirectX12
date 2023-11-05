#pragma once
#include "BaseAnimation.h"

/// <summary>
/// 待機アニメーションクラス
/// </summary>
class Idle : public BaseAnimation
{
public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime)override;

private: // メンバ変数

	// リターントリガー
	bool isReturn_;

	// 腕振り始端角度
	Vector3 armSwingStartAngle_;
	// 腕振り終端角度
	Vector3 armSwingEndAngle_;

	// 体振り始端角度
	Vector3 bodySwingStartAngle_;
	// 体振り終端角度
	Vector3 bodySwingEndAngle_;

};

