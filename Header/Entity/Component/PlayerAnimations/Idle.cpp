#include "Idle.h"

void Idle::Initialize()
{
	// 基底クラスの初期化
	BaseAnimation::Initialize();

	// t　初期化
	animT_ = 0.0f;
	animationTime_ = globalVariables_->Get<float>("Idle", "Idle_AnimationTime");

	// リターントリガーfalse
	isReturn_ = false;

	// グローバル変数から値を読む
	armSwingStartAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_ArmSwingStartAngle");
	armSwingEndAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_ArmSwingEndAngle");
	bodySwingStartAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_BodySwingStartAngle");
	bodySwingEndAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_BodySwingEndAngle");
}

void Idle::Update(float deltaTime)
{
	// 警告回避用
	deltaTime;

	// 自分自身のモデルコンポーネント取得
	auto* const modelComp = entity_->GetComponent<ModelComp>();

	// イージングによってアニメーション
	if (animT_ < animationTime_ / 2.0f) {
		if (isReturn_) {
			modelComp->GetBone("Arm_L")->transform_->rotate = AnimEasing::EaseInOut(animT_, armSwingStartAngle_, armSwingEndAngle_, animationTime_ / 2.0f);
			modelComp->GetBone("Arm_R")->transform_->rotate = AnimEasing::EaseInOut(animT_, armSwingStartAngle_, armSwingEndAngle_, animationTime_ / 2.0f);
			modelComp->GetBone("Body")->transform_->rotate = AnimEasing::EaseInOut(animT_, bodySwingStartAngle_, bodySwingEndAngle_, animationTime_ / 2.0f);
		}
		else {
			modelComp->GetBone("Arm_L")->transform_->rotate = AnimEasing::EaseInOut(animT_, armSwingEndAngle_, armSwingStartAngle_, animationTime_ / 2.0f);
			modelComp->GetBone("Arm_R")->transform_->rotate = AnimEasing::EaseInOut(animT_, armSwingEndAngle_, armSwingStartAngle_, animationTime_ / 2.0f);
			modelComp->GetBone("Body")->transform_->rotate = AnimEasing::EaseInOut(animT_, bodySwingEndAngle_, bodySwingStartAngle_, animationTime_ / 2.0f);
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

	// デバッグ時のみグローバル変数から値を読む
#ifdef _DEBUG
	// 適用
	animationTime_ = globalVariables_->Get<float>("Idle", "Idle_AnimationTime");
	armSwingStartAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_ArmSwingStartAngle");
	armSwingEndAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_ArmSwingEndAngle");
	bodySwingStartAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_BodySwingStartAngle");
	bodySwingEndAngle_ = globalVariables_->Get<Vector3>("Idle", "Idle_BodySwingEndAngle");
#endif // _DEBUG
}
