#include "FollowCameraComp.h"

void FollowCameraComp::Update() {

	camera_.translation_ = offset_ * Matrix4x4::EulerRotate(rotate_) + pTarget_->GetGrobalPos();
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
