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
private: // コンストラクタ、デストラクタ等

	// シングルトンパターンの設定
	CameraAnimManager() = default;
	~CameraAnimManager() = default;
	CameraAnimManager(const CameraAnimManager&) = delete;
	const CameraAnimManager& operator=(const CameraAnimManager&) = delete;

public: // メンバ関数

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

private: // メンバ変数

	// カメラマネージャーのインスタンス
	CameraManager* cameraManager_ = nullptr;

	// 現在再生中のアニメーション
	std::unique_ptr<CameraAnimation> currentAnimation_;
	// 再生待機中アニメーション
	std::vector<std::unique_ptr<CameraAnimation>> nextAnimation_;

	// 何秒後に再生するかタイマー
	SoLib::DeltaTimer timer_;
	// 遷移待機中トリガー
	bool transitionStandbyTrigger_ = false;

};

