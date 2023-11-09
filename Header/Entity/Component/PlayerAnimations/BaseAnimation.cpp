#include "BaseAnimation.h"

void BaseAnimation::Initialize(std::string name, bool isLoop, AnimEasing::EasingType type, float transitionTime)
{
	// グローバル変数クラスのインスタンス取得
	globalVariables_ = GlobalVariables::GetInstance();

	// ボーンの初期化
	bone_.Initialize(entity_, "PlayerBone", "Player");

	// 演出用t初期化
	animT_ = 0.0f;

	// 再生キーリセット
	playKey_ = 0;

	// 遷移秒数を取得
	transitionTime_ = transitionTime;
	transitionType_ = type;
	isTransitioning_ = false;
	// 遷移秒数が入力されていない、またはマイナスの場合は遷移を行わない
	if (transitionTime_ > 0) {
		isTransitioning_ = true;
		prevBone_ = GetPlayerBone(); // 現状のプレイヤー状態を取得しておく
	}

	animKeys_.Initialize(name);
	animKeys_.ApplyKeyInfo();

	isPlaying_ = true;
	isLoop_ = isLoop;
	isEnd_ = false;
}

void BaseAnimation::Update(float deltaTime)
{

	animKeys_.ApplyKeyInfo();
	if (isPlaying_) {
		if (isTransitioning_) {
			// イージングによってアニメーション
			if (animT_ < transitionTime_) {
				bone_.bone_ = Ease(transitionType_, animT_, prevBone_, animKeys_.keys_[0].bone.bone_, transitionTime_);
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
			if (animKeys_.keyCount_ > 1) {
				if (animT_ < animKeys_.keys_[playKey_].animationTime) {

					int targetKey = playKey_ + 1;
					if (playKey_ == animKeys_.keyCount_ - 1)
						if (isLoop_)
							targetKey = 0;

					bone_.bone_ = Ease(
						(AnimEasing::EasingType)animKeys_.keys_[playKey_].type, animT_,
						animKeys_.keys_[playKey_].bone.bone_, animKeys_.keys_[targetKey].bone.bone_, animKeys_.keys_[playKey_].animationTime);
					// 経過フレーム分加算
					animT_ += deltaTime;
				}
				else {
					if (isLoop_) {
						if (playKey_ < animKeys_.keyCount_ - 1) {
							playKey_++;
							animT_ = 0.0f;
						}
						else {
							animT_ = 0.0f;
							playKey_ = 0;
						}
					}
					else {
						if (playKey_ < animKeys_.keyCount_ - 2) {
							playKey_++;
							animT_ = 0.0f;
						}
						else {
							isEnd_ = true;
						}
					}
				}
			}
			else {
				bone_.bone_ = animKeys_.keys_[0].bone.bone_;
			}
		}
	}
	// 
	bone_.SetToEntity();
}

PlayerBone::Bone BaseAnimation::GetTargetBone(std::string groupName, std::string boneName)
{
	// 一時保存用
	PlayerBone::Bone tempBone;
	// グローバル変数から値を取得
	tempBone.body.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Body_Scale");
	tempBone.body.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Body_Rotate");
	tempBone.body.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Body_Translate");
	tempBone.eye.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Eye_Scale");
	tempBone.eye.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Eye_Rotate");
	tempBone.eye.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Eye_Translate");
	tempBone.ling.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Ling_Scale");
	tempBone.ling.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Ling_Rotate");
	tempBone.ling.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Ling_Translate");
	tempBone.arm_L.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_L_Scale");
	tempBone.arm_L.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_L_Rotate");
	tempBone.arm_L.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_L_Translate");
	tempBone.arm_R.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_R_Scale");
	tempBone.arm_R.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_R_Rotate");
	tempBone.arm_R.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Arm_R_Translate");
	tempBone.foot_L.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_L_Scale");
	tempBone.foot_L.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_L_Rotate");
	tempBone.foot_L.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_L_Translate");
	tempBone.foot_R.scale = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_R_Scale");
	tempBone.foot_R.rotate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_R_Rotate");
	tempBone.foot_R.translate = globalVariables_->Get<Vector3>(groupName, boneName + "Foot_R_Translate");

	// 結果を返す
	return tempBone;
}

PlayerBone::Bone BaseAnimation::GetPlayerBone()
{
	// 一時保存用
	PlayerBone::Bone tempBone;
	// プレイヤーのモデルコンポーネント取得
	auto* const modelComp = entity_->GetComponent<ModelComp>();
	// 構造体に
	tempBone.body.scale = modelComp->GetBone("Body")->transform_->scale;
	tempBone.body.rotate = modelComp->GetBone("Body")->transform_->rotate;
	tempBone.body.translate = modelComp->GetBone("Body")->transform_->translate;
	tempBone.eye.scale = modelComp->GetBone("Eye")->transform_->scale;
	tempBone.eye.rotate = modelComp->GetBone("Eye")->transform_->rotate;
	tempBone.eye.translate = modelComp->GetBone("Eye")->transform_->translate;
	tempBone.ling.scale = modelComp->GetBone("Ling")->transform_->scale;
	tempBone.ling.rotate = modelComp->GetBone("Ling")->transform_->rotate;
	tempBone.ling.translate = modelComp->GetBone("Ling")->transform_->translate;
	tempBone.arm_L.scale = modelComp->GetBone("Arm_L")->transform_->scale;
	tempBone.arm_L.rotate = modelComp->GetBone("Arm_L")->transform_->rotate;
	tempBone.arm_L.translate = modelComp->GetBone("Arm_L")->transform_->translate;
	tempBone.arm_R.scale = modelComp->GetBone("Arm_R")->transform_->scale;
	tempBone.arm_R.rotate = modelComp->GetBone("Arm_R")->transform_->rotate;
	tempBone.arm_R.translate = modelComp->GetBone("Arm_R")->transform_->translate;
	tempBone.foot_L.scale = modelComp->GetBone("Foot_L")->transform_->scale;
	tempBone.foot_L.rotate = modelComp->GetBone("Foot_L")->transform_->rotate;
	tempBone.foot_L.translate = modelComp->GetBone("Foot_L")->transform_->translate;
	tempBone.foot_R.scale = modelComp->GetBone("Foot_R")->transform_->scale;
	tempBone.foot_R.rotate = modelComp->GetBone("Foot_R")->transform_->rotate;
	tempBone.foot_R.translate = modelComp->GetBone("Foot_R")->transform_->translate;

	// 結果を返す
	return tempBone;
}

PlayerBone::Bone BaseAnimation::Ease(AnimEasing::EasingType type, float t, PlayerBone::Bone start, PlayerBone::Bone end, float time)
{
	PlayerBone::Bone tempBone;
	tempBone.Initilaize();
	switch (type)
	{
	case AnimEasing::kLinear:
		tempBone.body.scale = AnimEasing::Linear(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::Linear(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::Linear(t, start.body.translate, end.body.translate, time);
		tempBone.ling.scale = AnimEasing::Linear(t, start.ling.scale, end.ling.scale, time);
		tempBone.ling.translate = AnimEasing::Linear(t, start.ling.translate, end.ling.translate, time);
		tempBone.eye.scale = AnimEasing::Linear(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::Linear(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::Linear(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::Linear(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::Linear(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::Linear(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::Linear(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate = AnimEasing::Linear(t, start.arm_R.rotate, end.arm_R.rotate, time);
		tempBone.arm_R.translate = AnimEasing::Linear(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::Linear(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::Linear(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::Linear(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::Linear(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::Linear(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::Linear(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	case AnimEasing::kEaseIn:
		tempBone.body.scale = AnimEasing::EaseIn(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::EaseIn(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::EaseIn(t, start.body.translate, end.body.translate, time);
		tempBone.ling.scale = AnimEasing::EaseIn(t, start.ling.scale, end.ling.scale, time);
		tempBone.ling.translate = AnimEasing::EaseIn(t, start.ling.translate, end.ling.translate, time);
		tempBone.eye.scale = AnimEasing::EaseIn(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::EaseIn(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::EaseIn(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::EaseIn(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::EaseIn(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::EaseIn(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::EaseIn(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate = AnimEasing::EaseIn(t, start.arm_R.rotate, end.arm_R.rotate, time);
		tempBone.arm_R.translate = AnimEasing::EaseIn(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::EaseIn(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::EaseIn(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::EaseIn(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::EaseIn(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::EaseIn(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::EaseIn(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	case AnimEasing::kEaseOut:
		tempBone.body.scale = AnimEasing::EaseOut(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::EaseOut(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::EaseOut(t, start.body.translate, end.body.translate, time);
		tempBone.ling.scale = AnimEasing::EaseOut(t, start.ling.scale, end.ling.scale, time);
		tempBone.ling.translate = AnimEasing::EaseOut(t, start.ling.translate, end.ling.translate, time);
		tempBone.eye.scale = AnimEasing::EaseOut(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::EaseOut(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::EaseOut(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::EaseOut(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::EaseOut(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::EaseOut(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::EaseOut(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate = AnimEasing::EaseOut(t, start.arm_R.rotate, end.arm_R.rotate, time);
		tempBone.arm_R.translate = AnimEasing::EaseOut(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::EaseOut(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::EaseOut(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::EaseOut(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::EaseOut(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::EaseOut(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::EaseOut(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	case AnimEasing::KEaseInOut:
		tempBone.body.scale = AnimEasing::EaseInOut(t, start.body.scale, end.body.scale, time);
		tempBone.body.rotate = AnimEasing::EaseInOut(t, start.body.rotate, end.body.rotate, time);
		tempBone.body.translate = AnimEasing::EaseInOut(t, start.body.translate, end.body.translate, time);
		tempBone.ling.scale = AnimEasing::EaseInOut(t, start.ling.scale, end.ling.scale, time);
		tempBone.ling.translate = AnimEasing::EaseInOut(t, start.ling.translate, end.ling.translate, time);
		tempBone.eye.scale = AnimEasing::EaseInOut(t, start.eye.scale, end.eye.scale, time);
		tempBone.eye.rotate = AnimEasing::EaseInOut(t, start.eye.rotate, end.eye.rotate, time);
		tempBone.eye.translate = AnimEasing::EaseInOut(t, start.eye.translate, end.eye.translate, time);
		tempBone.arm_L.scale = AnimEasing::EaseInOut(t, start.arm_L.scale, end.arm_L.scale, time);
		tempBone.arm_L.rotate = AnimEasing::EaseInOut(t, start.arm_L.rotate, end.arm_L.rotate, time);
		tempBone.arm_L.translate = AnimEasing::EaseInOut(t, start.arm_L.translate, end.arm_L.translate, time);
		tempBone.arm_R.scale = AnimEasing::EaseInOut(t, start.arm_R.scale, end.arm_R.scale, time);
		tempBone.arm_R.rotate = AnimEasing::EaseInOut(t, start.arm_R.rotate, end.arm_R.rotate, time);
		tempBone.arm_R.translate = AnimEasing::EaseInOut(t, start.arm_R.translate, end.arm_R.translate, time);
		tempBone.foot_L.scale = AnimEasing::EaseInOut(t, start.foot_L.scale, end.foot_L.scale, time);
		tempBone.foot_L.rotate = AnimEasing::EaseInOut(t, start.foot_L.rotate, end.foot_L.rotate, time);
		tempBone.foot_L.translate = AnimEasing::EaseInOut(t, start.foot_L.translate, end.foot_L.translate, time);
		tempBone.foot_R.scale = AnimEasing::EaseInOut(t, start.foot_R.scale, end.foot_R.scale, time);
		tempBone.foot_R.rotate = AnimEasing::EaseInOut(t, start.foot_R.rotate, end.foot_R.rotate, time);
		tempBone.foot_R.translate = AnimEasing::EaseInOut(t, start.foot_R.translate, end.foot_R.translate, time);
		break;
	}

	return tempBone;
}
