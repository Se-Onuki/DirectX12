#include "FollowCameraComp.h"
#include "../../../Utils/SoLib/SoLib_Lerp.h"
#include "../../../Utils/SoLib/SoLib_ImGui.h"
#include "../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

void FollowCameraComp::Init() {
	camera_ = CameraManager::GetInstance()->AddCamera("FollowCamera");
	camera_->CalcMatrix();
}

void FollowCameraComp::Update() {

	Vector3 linePoint = SoLib::Lerp(line_.ClosestPoint(pTarget_->GetGrobalPos()), prePos_, vLerpValue);

	camera_->translation_ = offset_.GetItem() * Matrix4x4::EulerRotate(rotate_) + SoLib::Lerp(pTarget_->GetGrobalPos(), linePoint, vLerpValue) + addOffset_;
	camera_->translation_.y = offset_.GetItem().y + SoLib::Lerp(pTarget_->GetGrobalPos().y, linePoint.y, 0.25f) + addOffset_->y;

	Vector3 facing = linePoint - camera_->translation_;

	camera_->rotation_ = facing.Direction2Euler();

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
			rotate_ += euler;
			camera_->rotation_ += euler;
			camera_->UpdateMatrix();
		}
	}
}

void FollowCameraComp::SetTarget(BaseTransform *const target) {
	pTarget_ = target;
}

void FollowCameraComp::SetLine(const LineBase &target) {
	line_ = target;
}
