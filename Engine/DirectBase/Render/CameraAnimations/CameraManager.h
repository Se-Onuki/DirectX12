#pragma once
#include "CameraList.h"

/// <summary>
/// カメラアニメーションのマネージャー
/// </summary>
class CameraManager
{
private: // コンストラクタ、デストラクタ等

	// シングルトンパターンの設定
	CameraManager() = default;
	~CameraManager() = default;
	CameraManager(const CameraManager&) = delete;
	const CameraManager& operator=(const CameraManager&) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static CameraManager* GetInstance();

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update();

	/// <summary>
	/// ImGui描画関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// 使用中のカメラゲッター
	/// </summary>
	/// <returns>使用中のカメラ</returns>
	Camera3D* GetUseCamera() { return useCamera_; }

	/// <summary>
	/// 引数で指定した名前のカメラをメインカメラとして使用する
	/// </summary>
	/// <param name="cameraName">使用するカメラの名前</param>
	void SetUseCamera(const std::string& cameraName);

public: // パブリックなメンバ変数

	// カメラリスト
	std::unique_ptr<CameraList> cameraList_;

private: // メンバ変数

	// 使用中のカメラ
	Camera3D* useCamera_;

};

