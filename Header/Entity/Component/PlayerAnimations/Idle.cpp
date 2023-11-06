#include "Idle.h"

void Idle::Initialize(float transitionTime)
{
	// 基底クラスの初期化
	BaseAnimation::Initialize(transitionTime);

	// t　初期化
	animT_ = 0.0f;
	animationTime_ = globalVariables_->Get<float>("Idle", "Idle_AnimationTime");

	// リターントリガーfalse
	isReturn_ = true;

	// エンティティの現在状態を取得
	//bone_.Initialize(entity_);

	startBone_ = GetTargetBone("Idle", "IdleStart");
	endBone_ = GetTargetBone("Idle", "IdleEnd");
}

void Idle::Update(float deltaTime)
{
	if (isTransitioning_) {
		// イージングによってアニメーション
		if (animT_ < transitionTime_) {
			bone_.bone_ = AnimEasing::Ease(AnimEasing::KEaseInOut, animT_, prevBone_, startBone_, transitionTime_);
			// 経過フレーム分加算
			animT_ += deltaTime;
		}
		else {
			animT_ = 0.0f;
			// 遷移トリガーfalse
			isTransitioning_ = false;
		}
	}
	else {
		// イージングによってアニメーション
		if (animT_ < animationTime_ / 2.0f) {
			if (isReturn_) {
				bone_.bone_ = AnimEasing::Ease(AnimEasing::KEaseInOut, animT_, startBone_, endBone_, animationTime_ / 2.0f);
			}
			else {
				bone_.bone_ = AnimEasing::Ease(AnimEasing::KEaseInOut, animT_, endBone_, startBone_, animationTime_ / 2.0f);
			}

			// 経過フレーム分加算
			animT_ += deltaTime;
		}
		else {
			animT_ = 0.0f;

			if (isReturn_)
				isReturn_ = false;
			else
				isReturn_ = true;
		}
	}
	// 
	bone_.SetToEntity();

	// デバッグ時のみグローバル変数から値を読む
#ifdef _DEBUG
	// 適用
	startBone_ = GetTargetBone("Idle", "IdleStart");
	endBone_ = GetTargetBone("Idle", "IdleEnd");
	animationTime_ = globalVariables_->Get<float>("Idle", "Idle_AnimationTime");

#endif // _DEBUG
}
