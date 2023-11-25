#include "CameraAnimManager.h"
#include "CameraManager.h"

CameraAnimManager* CameraAnimManager::GetInstance()
{
	static CameraAnimManager instance;
	return &instance;
}

void CameraAnimManager::Init()
{
	// カメラマネージャーのインスタンス取得
	cameraManager_ = CameraManager::GetInstance();

	// シーンチェンジ
	SceneChange();

	// 再生中アニメーションのインスタンスを作る
	//currentAnimation_ = std::make_unique<CameraAnimation>(); // インスタンス生成
}

void CameraAnimManager::Update(float deltaTime)
{
	// 現在再生中アニメーションが終了していなければ
	if (!currentAnimation_.GetIsEnd()) {
		isPlaying_ = true;
		currentAnimation_.Update(deltaTime); // 更新処理を呼び出す
	}
	else {
		isPlaying_ = false;
	}

	// 再生待機アニメーションが１つでもあれば
	if (nextAnimations_.size() > 0) {
		// 次のアニメーションが終了まで待機するか
		if (nextAnimations_.front().standByIsEnd) {
			// 現在アニメーションが終了している、遷移待機中でない場合
			if (currentAnimation_.GetIsEnd() && !transitionStandbyTrigger_) {
				// タイマー開始
				timer_.Start(nextAnimations_.front().standByTime);
				// 遷移待機中
				transitionStandbyTrigger_ = true;
			}
		}
		else {
			if (!transitionStandbyTrigger_) {
				// タイマー開始
				timer_.Start(nextAnimations_.front().standByTime);
				// 遷移待機中
				transitionStandbyTrigger_ = true;
			}
		}

		if (timer_.IsFinish() && transitionStandbyTrigger_) {
			// 現在アニメーションに次のアニメーションを設定
			currentAnimation_ = nextAnimations_.front().anim;
			// 渡した要素を削除
			nextAnimations_.erase(nextAnimations_.begin());
			// 遷移待機中ではない
			transitionStandbyTrigger_ = false;
		}
	}

	// 遷移待機タイマー更新
	timer_.Update(deltaTime);
}

bool CameraAnimManager::GetIsPlaying()
{
	if (isPlaying_ && !currentAnimation_.GetIsEnd()) {
		return true;
	}
	return false;
}

void CameraAnimManager::SceneChange()
{
	// 再生待機中アニメーションリストをクリア
	nextAnimations_.clear();
	// 現在アニメーションを強制終了
	currentAnimation_.End();
}

void CameraAnimManager::Play(Camera3D* endCamera, float time, float(*func)(float), float standByTime, bool standByIsEnd)
{
	// 次のアニメーションインスタンス生成
	NextAnimation nextAnimation;
	// アニメーション本体の生成
	nextAnimation.anim.Init(endCamera, time, func); // 初期化
	// パラメーター設定
	nextAnimation.standByTime = standByTime;		 // 待機秒数設定
	nextAnimation.standByIsEnd = standByIsEnd;		 // 現在アニメーション終了まで待機するか

	// 再生待機アニメーションリストに追加
	nextAnimations_.push_back(nextAnimation);
}
