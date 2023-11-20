#pragma once
#include "CameraList.h"
#include "CameraManager.h"
#include "../../../../Utils/SoLib/SoLib.h"

/// <summary>
/// カメラのアニメーション
/// </summary>
class CameraAnimation
{
public: // コンストラクタ等

	// コンストラクタ
	CameraAnimation() = default;
	// デストラクタ
	~CameraAnimation() = default;

public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="endCamera"終端カメラ</param>
	/// <param name="time">アニメーション秒数</param>
	/// <param name="func">線形補間の型</param>
	void Initialize(Camera3D* endCamera, float time, float(*func)(float) = SoLib::easeLinear);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime);

private: // メンバ変数

	// パラメータ格納用カメラ
	std::unique_ptr<Camera3D> camera_;
	// 終端パラメータ保管用
	Camera3D* endCamera_;

	// アニメーション時間
	SoLib::DeltaTimer timer_;
};

