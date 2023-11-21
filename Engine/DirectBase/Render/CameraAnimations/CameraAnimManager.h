#pragma once
#include "CameraAnimation.h"
#include <vector>

// クラスの前方宣言
class CameraManager;

/// <summary>
/// カメラアニメーションのマネージャー
/// </summary>
class CameraAnimManager
{
private: // サブクラス

	/// <summary>
	/// 次のアニメーション構造体
	/// </summary>
	struct NextAnimation {
		CameraAnimation anim; // アニメーション本体
		float standByTime;					   // 次のアニメーションまでの待機秒数
		bool standByIsEnd;					   // 現在アニメーションが終了するまで待機するか
	};

private: // コンストラクタ、デストラクタ等

	// シングルトンパターンの設定
	CameraAnimManager() = default;
	~CameraAnimManager() = default;
	CameraAnimManager(const CameraAnimManager&) = delete;
	const CameraAnimManager& operator=(const CameraAnimManager&) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static CameraAnimManager* GetInstance();

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

public: // その他関数

	/// <summary>
	/// カメラアニメーション再生関数
	/// </summary>
	/// <param name="endCamera">終端パラメーター</param>
	/// <param name="time">アニメーション秒数</param>
	/// <param name="func">イージングタイプ(Solib::linear 等)</param>
	/// <param name="standByTime">待機秒数</param>
	/// <param name="standByIsEnd">現在のアニメーションが終了するまで待機するか</param>
	void Play(Camera3D* endCamera, float time, float(*func)(float), float standByTime, bool standByIsEnd = false);

private: // メンバ変数

	// カメラマネージャーのインスタンス
	CameraManager* cameraManager_ = nullptr;

	// 現在再生中のアニメーション
	CameraAnimation currentAnimation_;
	// 再生待機中アニメーション
	std::list<NextAnimation> nextAnimations_;

	// 何秒後に再生するかタイマー
	SoLib::DeltaTimer timer_;
	// 遷移待機中トリガー
	bool transitionStandbyTrigger_ = false;

};

