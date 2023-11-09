#include "FollowCameraComp.h"
#include "../../../Utils/SoLib/SoLib_Lerp.h"

void FollowCameraComp::Update() {

	Vector3 linePoint = line_.ClosestPoint(pTarget_->GetGrobalPos());

	camera_.translation_ = offset_.GetItem() * Matrix4x4::EulerRotate(rotate_) + SoLib::Lerp(pTarget_->GetGrobalPos(), linePoint, vLerpValue);

	camera_.UpdateMatrix();
}

void FollowCameraComp::AddRotate(const Vector3 &euler) {
	rotate_ += euler;
	camera_.rotation_ = rotate_;
	camera_.UpdateMatrix();
}

void FollowCameraComp::SetTarget(BaseTransform *const target) {
	pTarget_ = target;
}

void FollowCameraComp::SetLine(const LineBase &target) {
	line_ = target;
}
