#include "FollowCamera.h"
#include "../../DirectBase/Input/Input.h"
#include "../Math/Transform.h"

void FollowCamera::Init() {
	viewProjection_.Init();
	input_ = Input::GetInstance();
}

void FollowCamera::Update() {
	if (target_) {
		const VirtualPad *const vPad = input_->GetXInput()->GetState();
		Vector3 offset{ 3.f, 2.f, -13.f };

		if (vPad->stickR_.Length() > 0.1f) {
			const float rotateStick = vPad->stickR_.x;
			rotate_.y += rotateStick * cameraRotSpeed_;
		}
		const Matrix4x4 &mat = Matrix4x4::EulerRotate(Matrix4x4::Yaw, rotate_.y);

		offset = TransformNormal(offset, mat);

		viewProjection_.rotation_.y = rotate_.y;

		viewProjection_.translation_ = target_->translate + offset;
	}
	viewProjection_.UpdateMatrix();
}

const Matrix4x4 &FollowCamera::GetViewMatrix() const {
	return viewProjection_.matView_;
}

const Matrix4x4 &FollowCamera::GetProjectionMatrix() const {
	return viewProjection_.matProjection_;
}

const ViewProjection *const FollowCamera::GetViewProjection() const {
	return &viewProjection_;
}
