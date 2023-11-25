#pragma once
#include "../../../../Utils/SoLib/SoLib.h"
#include "../../../../Utils/Math/Angle.h"
#include "../Camera.h"

/// <summary>
/// カメラのアニメーション
/// </summary>
class CameraAnimation
{
private: // サブクラス

	/// <summary>
	/// カメラパラメータ補間用構造体
	/// </summary>
	struct CameraParameter {
		Vector3 translation;
		Vector3 rotation;
		float FOV;
	};

public: // コンストラクタ等

	// コンストラクタ
	CameraAnimation() = default;
	// デストラクタ
	~CameraAnimation() = default;

public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="endCamera"終端パラメータを持つカメラ</param>
	/// <param name="time">アニメーション秒数</param>
	/// <param name="func">線形補間の型</param>
	void Init(Camera3D* endCamera, float time, float(*func)(float) = SoLib::easeLinear);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime);

public: // アクセッサ等

	/// <summary>
	/// 強制終了関数
	/// </summary>
	void Finish() { timer_.Start(0.0f); }

	/// <summary>
	///	アニメーション終了状態ゲッター
	/// </summary>
	/// <returns>アニメーションが終了しているか</returns>
	bool GetIsEnd() { return isEnd_; }

	/// <summary>
	/// アニメーション強制終了
	/// </summary>
	void End();

private: // メンバ変数

	// パラメータ格納用カメラ
	static std::unique_ptr<Camera3D> camera_;
	
	// 始端パラメーター保管用
	CameraParameter startCamera_;
	// 終端パラメータ保管用
	Camera3D* endCamera_;

	// アニメーション時間
	SoLib::DeltaTimer timer_;

	// 終了トリガー
	bool isEnd_ = true;

	// イージングの関数ポインタ
	float(*Ease)(float) = nullptr;
};

