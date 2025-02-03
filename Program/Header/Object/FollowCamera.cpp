/// @file FollowCamera.cpp
/// @brief フォローカメラ
/// @author ONUKI seiya
#include "FollowCamera.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Engine/Utils/Math/Transform.h"
#include <Xinput.h>

void FollowCamera::Init() {
	camera_.Init();
	input_ = SolEngine::Input::GetInstance();
}

void FollowCamera::Update() {
	if (target_) {

		Vector3 offset{ 0.f, 3.f, -15.f };
		const auto &vPad = input_->GetXInput()->GetState();

		const float rotateStick = vPad->stickL_.x;
		rotate_.y += rotateStick * cameraRotSpeed_;

		const Matrix4x4 &mat = Matrix4x4::EulerRotate(Matrix4x4::Yaw, rotate_.y);

		offset = TransformNormal(offset, mat);

		camera_.rotation_.y = rotate_.y;

		camera_.translation_ = (*target_)->translate + offset;
	}
	camera_.UpdateMatrix();
}

const Matrix4x4 &FollowCamera::GetViewMatrix() const { return camera_.matView_; }

const Matrix4x4 &FollowCamera::GetProjectionMatrix() const { return camera_.matProjection_; }
