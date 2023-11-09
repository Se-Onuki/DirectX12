#include "FollowCameraComp.h"
#include "../../../Utils/SoLib/SoLib_Lerp.h"
#include "../../../Utils/SoLib/SoLib_ImGui.h"

void FollowCameraComp::Update() {

	Vector3 linePoint = line_.ClosestPoint(pTarget_->GetGrobalPos());

	camera_.translation_ = offset_.GetItem() * Matrix4x4::EulerRotate(rotate_) + SoLib::Lerp(pTarget_->GetGrobalPos(), linePoint, vLerpValue) + addOffset_;

	Vector3 facing = linePoint - camera_.translation_;

	camera_.rotation_ = facing.Direction2Euler();

	camera_.UpdateMatrix();
}

void FollowCameraComp::ImGuiWidget() {
	SoLib::ImGuiWidget(&vLerpValue);
	SoLib::ImGuiWidget(&offset_);
	SoLib::ImGuiWidget(&addOffset_);
}

void FollowCameraComp::AddRotate(const Vector3 &euler) {
	if (euler.LengthSQ() != 0.f) {
		rotate_ += euler;
		camera_.rotation_ += euler;
		camera_.UpdateMatrix();
	}
}

void FollowCameraComp::SetTarget(BaseTransform *const target) {
	pTarget_ = target;
}

void FollowCameraComp::SetLine(const LineBase &target) {
	line_ = target;
}
