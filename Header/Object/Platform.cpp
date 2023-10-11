#include "Platform.h"
#include "../../Engine/DirectBase/Model/Model.h"

void Platform::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);

	transformOrigin_.translate = centor_;

	transformOrigin_.CalcMatrix();

	modelTransform_.SetParent(transformOrigin_);
	modelTransform_.scale = { 10.f,0.1f,10.f };
	modelTransform_.CalcMatrix();

	CalcCollider();
}

void Platform::Update() {
	// 動作フラグが有効ならば
	if (isMove_) {
		// 時間を追加
		++nowTime_;

		float angle = Angle::Mod((static_cast<float>(nowTime_) / needTime_) * Angle::Dig360);

		Vector3 diff = offset_ * Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, angle);

		transformOrigin_.rotate.y = angle;
		transformOrigin_.translate = centor_ + diff;

		transformOrigin_.CalcMatrix();
	}
	modelTransform_.UpdateMatrix();
	CalcCollider();
}

void Platform::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	for (auto &model : modelMap_) {
		model.second->Draw(modelTransform_, camera);
	}
}

void Platform::CalcCollider() {
	const SRT &srt = SRT::MatToSRT(modelTransform_.matWorld_);

	collider_.size = srt.scale;
	collider_.SetRotate(srt.rotate);
	collider_.centor = srt.translate;
}
