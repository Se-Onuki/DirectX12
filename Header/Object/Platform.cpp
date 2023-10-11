#include "Platform.h"

void Platform::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);

	transformOrigin_.scale = { 10.f,0.1f,10.f };
	transformOrigin_.translate = centor_;

	transformOrigin_.UpdateMatrix();

	CalcCollider();
}

void Platform::Update() {
	// 動作フラグが有効ならば
	if (isMove_) {
		// 時間を追加
		++nowTime_;

		float angle = (static_cast<float>(nowTime_) / needTime_) * Angle::Dig360;

		Vector3 diff = offset_ * Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, angle);

		transformOrigin_.rotate.y = angle;
		transformOrigin_.translate = centor_ + diff;

		transformOrigin_.UpdateMatrix();
		CalcCollider();
	}
}

void Platform::CalcCollider() {
	const SRT &srt = SRT::MatToSRT(transformOrigin_.matWorld_);

	collider_.size = srt.scale;
	collider_.SetRotate(srt.rotate);
	collider_.centor = srt.translate;
}
