#include "CameraList.h"

void CameraList::ListClear()
{
	// マップのクリア
	cameraMap_.clear();
}

Camera3D* CameraList::AddCamera(const std::string& cameraName)
{
	// return用のインスタンス生成
	Camera3D* camera = nullptr;

	// カメラリスト内にキーと同じ名前のカメラがあるかチェック
	auto cameraItr = cameraMap_.find(cameraName);
	// 存在していた場合リストへの追加を行わず、そのカメラを返す
	if (cameraItr != cameraMap_.end()) {
		camera = cameraItr->second.get();
		return camera;
	}

	// 存在していなかった場合カメラをリストに追加
	// インスタンスの生成
	std::unique_ptr<Camera3D> newCamera = std::make_unique<Camera3D>();

	// return用のインスタンス生成
	//camera = newCamera.get();

	// カメラリストに生成したカメラを追加
	cameraMap_.insert({ cameraName, std::move(newCamera) });

	// 生成したカメラを返す
	return cameraMap_[cameraName].get();
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
