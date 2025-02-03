/// @file CameraList.h
/// @brief カメラのリスト
/// @author TOYODA yuki
#pragma once
#include "../Camera.h"
#include <map>

/// <summary>
/// 使用中のカメラリスト
/// </summary>
class CameraList
{
public: // コンストラクタ等

	// コンストラクタ
	CameraList() = default;
	// デストラクタ
	~CameraList() = default;

public: // メンバ関数

	/// <summary>
	/// 読み込まれているカメラのリストをクリアする関数
	/// </summary>
	void ListClear();

	/// <summary>
	/// カメラをリストに追加する関数
	/// </summary>
	/// <param name="cameraName">追加するカメラ名称</param>
	/// <returns>生成したカメラのポインタ</returns>
	SolEngine::Camera3D* AddCamera(const std::string& cameraName);

	/// <summary>
	/// カメラをリストから取得する関数
	/// </summary>
	/// <param name="cameraName">取得するカメラ名称</param>
	/// <returns>カメラ（ ない場合はnullptr ）</returns>
	SolEngine::Camera3D* GetCamera(const std::string& cameraName) const;

public: // メンバ変数

	// カメラの配列
	std::unordered_map<std::string, std::unique_ptr<SolEngine::Camera3D>> cameraMap_;

};

