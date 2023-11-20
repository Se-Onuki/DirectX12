#include "CameraList.h"

void CameraList::ListClear()
{
	// マップのクリア
	cameraMap_.clear();
}

Camera3D* CameraList::AddCamera(std::string cameraName)
{
	// インスタンスの生成
	std::unique_ptr<Camera3D> newCamera = std::make_unique<Camera3D>();

	// return用のインスタンス生成
	Camera3D* camera = newCamera.get();

	// カメラリストに生成したカメラを追加
	cameraMap_.insert({ cameraName, std::move(newCamera) });

	// 生成したカメラを返す
	return camera;
}

Camera3D* CameraList::GetCamera(std::string cameraName)
{
	// マップから情報を取得し返す
	return cameraMap_[cameraName].get();
}
