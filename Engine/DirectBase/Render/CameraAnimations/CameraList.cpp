#include "CameraList.h"

void CameraList::ListClear()
{
	// マップのクリア
	cameraMap_.clear();
}

Camera3D* CameraList::AddCamera(const std::string& cameraName)
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

Camera3D* CameraList::GetCamera(const std::string& cameraName) const
{
	// イテレータの取得
	auto cameraItr = cameraMap_.find(cameraName);
	// 存在するかチェック
	if (cameraItr == cameraMap_.end()) {
		return nullptr;
	}

	// マップから情報を取得し返す
	return cameraItr->second.get();
}
