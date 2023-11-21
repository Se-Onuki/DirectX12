#include "CameraAnimation.h"
#include "CameraManager.h"


void CameraAnimation::Init(Camera3D* endCamera, float time, float(*func)(float))
{
	// パラメータを格納する
	camera_ = std::make_unique<Camera3D>();											   // インスタンス生成
	*camera_ = *CameraManager::GetInstance()->GetUseCamera(); // 使用中カメラのパラメータを取得

	// 始端パラメータを取得
	startCamera_ = *CameraManager::GetInstance()->GetUseCamera();
	// 終端パラメータを取得
	endCamera_ = endCamera;

	// タイマー設定
	timer_.Start(time);

	// イージングのタイプを取得
	Ease = func;
}

void CameraAnimation::Update(float deltaTime)
{
	// タイマーが終了していなければ
	if (!timer_.IsFinish()) {
		// カメラのパラメーターを線形補間によって動かす
		camera_->translation_ = SoLib::Lerp(startCamera_.translation_, endCamera_->translation_, Ease(timer_.GetProgress())); // 位置座標
		camera_->rotation_ = Angle::Lerp(startCamera_.rotation_, endCamera_->rotation_, Ease(timer_.GetProgress()));		   // 回転
		camera_->fovAngleY = SoLib::Lerp(startCamera_.fovAngleY, endCamera_->fovAngleY, Ease(timer_.GetProgress()));		   // FOV
	}
	else { // 終了したら
		// カメラのパラメーターを終了時のパラメーターに合わせる
		*camera_ = *endCamera_;
	}

	// 使用されているカメラにパラメーターをセット
	CameraManager::GetInstance()->SetParameters(*camera_);
	
	// タイマーの更新
	timer_.Update(deltaTime);
}
