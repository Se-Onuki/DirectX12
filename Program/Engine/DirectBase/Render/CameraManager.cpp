#include "CameraManager.h"

namespace SolEngine {
	void CameraManager::Init()
	{
		AddCamera("MainCamera");
		SetMainCamera("MainCamera");
	}
	void CameraManager::Clear()
	{
		cameras_.clear();
		mainCamera_ = cameras_.end();
	}
	Camera3D *const CameraManager::GetCamera() const
	{
		return (mainCamera_ != cameras_.end() ? mainCamera_->second.get() : nullptr);
	}

	Camera3D *const CameraManager::GetCamera(const std::string &key) const
	{
		// キーを取得
		auto itr = cameras_.find(key);

		// もし有効な値があったら返す
		return (itr != cameras_.end() ? itr->second.get() : nullptr);
	}

	Camera3D *const CameraManager::AddCamera(const std::string &key)
	{
		auto camera = std::make_unique<Camera3D>();
		camera->Init();

		return cameras_.emplace(key, std::move(camera)).first->second.get();
	}
	Camera3D *const CameraManager::AddCamera(const std::string &key, std::unique_ptr<Camera3D> &&camera)
	{
		return cameras_.emplace(key, std::move(camera)).first->second.get();
	}

	void CameraManager::SetMainCamera(const std::string &key)
	{

		// キーを取得
		auto itr = cameras_.find(key);

		// もし有効な値だったら設定する
		if (itr != cameras_.end()) {
			mainCamera_ = itr;
		}

	}

	void CameraManager::CalcAll()
	{
		for (auto &camera : cameras_) {
			camera.second->UpdateMatrix();
		}
	}

}