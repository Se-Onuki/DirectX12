#include "CameraManager.h"

CameraManager* CameraManager::GetInstance()
{
	static CameraManager instance;
	return &instance;
}

void CameraManager::Initialize()
{
	// 読み込まれているカメラをクリア
	cameraList_->ListClear();

	// 最初のカメラを追加
	cameraList_->AddCamera("DebugCamera");

}

void CameraManager::Update()
{

	for (auto& camera : cameraList_->cameraMap_) {
		camera.second->CalcMatrix();
	}

	if (useCamera_ != nullptr) {
		useCamera_->TransferMatrix();
	}
}

void CameraManager::SetUseCamera(const std::string& cameraName)
{
	Camera3D* getCamera = cameraList_->GetCamera(cameraName);
	if (getCamera != nullptr) {
		useCamera_ = getCamera;
	}
}
