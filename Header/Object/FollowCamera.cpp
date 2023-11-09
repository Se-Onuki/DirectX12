#include "FollowCamera.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Utils/Math/Transform.h"
#include <Xinput.h>

void FollowCamera::Init() {
	camera_.Init();
	input_ = Input::GetInstance();
}

void FollowCamera::Update() {
	if (target_) {

		Vector3 offset{ 0.f, 3.f, -15.f };
		const auto &vPad = input_->GetXInput()->GetState();

		const float rotateStick = vPad->stickR_.x;
		if (std::abs(rotateStick) >= 0.1f) {
			rotate_.y += rotateStick * cameraRotSpeed_;
		}
		const Matrix4x4 &mat = Matrix4x4::EulerRotate(Matrix4x4::Yaw, rotate_.y);

		offset = TransformNormal(offset, mat);

		camera_.rotation_.y = rotate_.y;

		camera_.translation_ = target_->GetGrobalPos() + offset;

	}
	camera_.UpdateMatrix();
}

const Matrix4x4 &FollowCamera::GetViewMatrix() const { return camera_.matView_; }

const Matrix4x4 &FollowCamera::GetProjectionMatrix() const { return camera_.matProjection_; }

bool FollowCamera::ImGuiWidget() {

	if (ImGui::TreeNode("FollowCamera")) {
		bool isUsing = false;

		isUsing |= ImGui::DragFloat3("Rotate", &rotate_.x, Angle::Dig2Rad);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}
