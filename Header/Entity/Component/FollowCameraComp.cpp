#include "FollowCameraComp.h"

void FollowCameraComp::Update() {
	/*pTarget_->Get

	camera_.translation_ + */
}

void FollowCameraComp::AddRotate(const Vector3 &euler) {
	camera_.rotation_ += euler;
	camera_.UpdateMatrix();
}

void FollowCameraComp::SetTarget(BaseTransform *const target) {
	pTarget_ = target;
}
