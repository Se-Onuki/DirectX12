#include "CameraAnimation.h"
#include "CameraManager.h"

// 静的なメンバ変数の実体を宣言
std::unique_ptr<Camera3D> CameraAnimation::camera_;

void CameraAnimation::Init(Camera3D* endCamera, float time, float(*func)(float))
{
	if (camera_ == nullptr) {
		camera_ = std::make_unique<Camera3D>();
	}

	// パラメータを格納する
	//*camera_ = *CameraManager::GetInstance()->GetUseCamera(); // 使用中カメラのパラメータを取得

	// 始端パラメータを取得
	startCamera_.translation = CameraManager::GetInstance()->GetUseCamera()->translation_;
	startCamera_.rotation = CameraManager::GetInstance()->GetUseCamera()->rotation_;
	startCamera_.FOV = CameraManager::GetInstance()->GetUseCamera()->fovAngleY;
	// 終端パラメータを取得
	endCamera_ = endCamera;

	// タイマー設定
	timer_.Start(time);

	// 切り替え前に更新
	camera_->translation_ = startCamera_.translation;
	camera_->rotation_ = startCamera_.rotation;
	camera_->fovAngleY = startCamera_.FOV;

	camera_->UpdateMatrix();
	CameraManager::GetInstance()->SetUseCamera(camera_.get());

	// イージングのタイプを取得
	Ease = func;

	// アニメーション終了
	isEnd_ = false;
}

void CameraAnimation::Update(float deltaTime)
{
	// タイマーが終了していなければ
	if (not timer_.IsFinish()) {
		// カメラのパラメーターを線形補間によって動かす
		camera_->translation_ = SoLib::Lerp(startCamera_.translation, endCamera_->translation_, Ease(timer_.GetProgress())); // 位置座標
		camera_->rotation_ = Angle::Lerp(startCamera_.rotation, endCamera_->rotation_, Ease(timer_.GetProgress()));		   // 回転
		camera_->fovAngleY = SoLib::Lerp(startCamera_.FOV, endCamera_->fovAngleY, Ease(timer_.GetProgress()));		   // FOV
	}
	else { // 終了したら
		// カメラのパラメーターを終了時のパラメーターに合わせる
		CameraManager::GetInstance()->SetUseCamera(endCamera_);
		// アニメーション終了
		isEnd_ = true;
	}
	// タイマーの更新
	timer_.Update(deltaTime);

	// カメラの更新
	camera_->UpdateMatrix();
}

void CameraAnimation::End()
{
	// アニメーション終了
	isEnd_ = true;
}
