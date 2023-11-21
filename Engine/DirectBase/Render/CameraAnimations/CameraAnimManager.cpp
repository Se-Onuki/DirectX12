#include "CameraAnimManager.h"
#include "CameraManager.h"

void CameraAnimManager::Init()
{
	// カメラマネージャーのインスタンス取得
	cameraManager_ = CameraManager::GetInstance();

	// 再生中アニメーションのインスタンスを作る
	currentAnimation_ = std::make_unique<CameraAnimation>(); // インスタンス生成
	currentAnimation_->Init(cameraManager_->cameraList_->GetCamera("DebugCamera"), 0.0f); // 初期化


}

void CameraAnimManager::Update(float deltaTime)
{
	// 現在再生中アニメーションが終了していなければ
	if (!currentAnimation_->GetIsEnd()) {
		currentAnimation_->Update(deltaTime); // 更新処理を呼び出す
	}
}
