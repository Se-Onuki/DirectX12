#include "CameraManager.h"

CameraManager* CameraManager::GetInstance()
{
	static CameraManager instance;
	return &instance;
}

void CameraManager::Init()
{
	// カメラリストがnullptrなら
	if (cameraList_ == nullptr) {
		cameraList_ = std::make_unique<CameraList>();
	}

	// 読み込まれているカメラをクリア
	cameraList_->ListClear();

	// 最初のカメラを追加
	cameraList_->AddCamera("DebugCamera");
	SetUseCamera("DebugCamera");
}

void CameraManager::Update()
{
	// リスト内の全てのカメラの計算を行う
	for (auto& camera : cameraList_->cameraMap_) {
		// 計算を行う
		camera.second->CalcMatrix();
	}

	// 使用カメラがあるなら
	if (useCamera_ != nullptr) {
		// 計算した内容を転送する
		useCamera_->TransferMatrix();
	}
}

void CameraManager::DisplayImGui()
{
	ImGui::Begin("CameraManager");
	// 全カメラののImGuiを描画
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(0, 100), ImGuiWindowFlags_NoTitleBar);
	// イテレータの取得
	auto iter = cameraList_->cameraMap_.begin();
	// イテレータの終わりまでループ
	while (iter != cameraList_->cameraMap_.end()) {
		ImGui::Text(iter->first.c_str());
		if (useCamera_ == iter->second.get()) {
			ImGui::Text("ThisCamera is used");
			iter->second->ImGuiWidget();
		}
		iter++;
	}
	
	ImGui::EndChild();
	ImGui::End();
}

void CameraManager::SetUseCamera(const std::string& cameraName)
{
	// カメラ配列からカメラを取得
	Camera3D* getCamera = cameraList_->GetCamera(cameraName);
	// 取得できた場合はそのカメラを使用する
	if (getCamera != nullptr) {
		useCamera_ = getCamera;
	}
}
