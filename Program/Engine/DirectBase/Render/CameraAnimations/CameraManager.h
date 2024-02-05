#pragma once
#include "CameraAnimManager.h"
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
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// ImGui描画関数
	/// </summary>
	void DisplayImGui();

public: // アクセッサ等

	/// <summary>
	/// カメラリストにカメラを追加する関数
	/// </summary>
	/// <param name="cameraName">追加するカメラ名</param>
	/// <returns>追加したカメラ</returns>
	Camera3D* AddCamera(const std::string& cameraName);

	Camera3D* GetCamera(const std::string& cameraName) const;

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

	/// <summary>
	/// 
	/// </summary>
	/// <param name="camera"></param>
	void SetUseCamera( Camera3D* camera);

	/// <summary>
	/// カメラに引数のパラメーター数値を渡す関数
	/// </summary>
	/// <param name="parameter">セットするパラメーター</param>
	void SetParameters(const Camera3D& parameter) { *useCamera_ = parameter; }

	CameraAnimManager* GetCameraAnimManager(){return cameraAnimManager_;}

public: // パブリックなメンバ変数

	// カメラリスト
	std::unique_ptr<CameraList> cameraList_;

private: // メンバ変数

	// カメラアニメーションマネージャー
	CameraAnimManager* cameraAnimManager_ = nullptr;

	// 使用中のカメラ
	Camera3D* useCamera_;

#ifdef USE_IMGUI // ImGui用メンバ変数

	// 終端値のパラメーターを取得するカメラ名格納用
	char imGuiEndCameraName_[20];

	// アニメーション再生秒数
	float imGuiPlayAnimTime_ = 1.0f;

	// 現在アニメーション終了まで待機するかトリガー
	bool imGuiStandByIsEnd_ = false;
	// 待機秒数
	float imGuiStandByTime_ = 0.0f;

#endif // USE_IMGUI // ImGui用メンバ変数


};

