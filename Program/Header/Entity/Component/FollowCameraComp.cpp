/// @file FollowCameraComp.cpp
/// @brief フォローカメラのコンポーネント
/// @author ONUKI seiya
#include "FollowCameraComp.h"
#include "../../Engine/Utils/SoLib/SoLib_Lerp.h"
#include "../../Engine/Utils/SoLib/SoLib_ImGui.h"
#include "../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

void FollowCameraComp::Init() {
	camera_ = CameraManager::GetInstance()->AddCamera("FollowCamera");
	camera_->CalcMatrix();
}

void FollowCameraComp::Update() {

	Vector3 linePoint = SoLib::Lerp(line_.ClosestPoint(pTarget_->GetGrobalPos()), prePos_, vLerpValue);


	if (rotate_.x > 15._deg) {
		rotate_.x = SoLib::Lerp(rotate_.x, 15._deg, 0.2f);
	}
	else if (rotate_.x < -40._deg) {
		rotate_.x = SoLib::Lerp(rotate_.x, -40._deg, 0.2f);
	}

	SoLib::ImGuiDraw("FollowCameraAngle", SoLib::to_string(rotate_));

	const Vector3 cameraOffset = offset_.GetItem() * rotate_;

	camera_->translation_ = cameraOffset + SoLib::Lerp(pTarget_->GetGrobalPos(), linePoint, vLerpValue);
	camera_->translation_.y = cameraOffset.y + SoLib::Lerp(pTarget_->GetGrobalPos().y, linePoint.y, 0.25f) + addOffset_->y;

	Vector3 facing = linePoint - camera_->translation_;

	camera_->rotation_ = Quaternion::LookAt(facing);

	camera_->UpdateMatrix();

	prePos_ = linePoint;
}

void FollowCameraComp::ImGuiWidget() {
	SoLib::ImGuiWidget(&vLerpValue);
	SoLib::ImGuiWidget(&offset_);
	SoLib::ImGuiWidget(&addOffset_);
}

void FollowCameraComp::AddRotate(const Vector3 &euler) {
	if (euler.LengthSQ() != 0.f) {
		if (CameraManager::GetInstance()->GetUseCamera() == camera_) {
			rotate_ *= Quaternion::LookAt(SoLib::EulerToDirection(euler));

			camera_->rotation_ *= Quaternion::LookAt(SoLib::EulerToDirection(euler));
			camera_->CalcMatrix();
		}
	}
}

void FollowCameraComp::SetTarget(BaseTransform *const target) {
	pTarget_ = target;
}

void FollowCameraComp::SetLine(const LineBase &target) {
	line_ = target;
}
