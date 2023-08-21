#include "FollowCamera.h"
#include "../../DirectBase/Input/Input.h"
#include "../Math/Transform.h"

#include "../../externals/imgui/imgui.h"

void FollowCamera::Init() {
	viewProjection_.Init();
	input_ = Input::GetInstance();
}

void FollowCamera::Update() {
	if (target_) {
		const VirtualPad *const vPad = input_->GetXInput()->GetState();
		Vector3 offset;

		if (vPad->stickR_.Length() > 0.1f) {
			rotate_.y += vPad->stickR_.x * cameraRotSpeed_.y; // 横方向
			rotate_.x += -vPad->stickR_.y * cameraRotSpeed_.x;	// 縦方向
		}
		// 埋まりこみ対策
		if (rotate_.x < minRotate_) { rotate_.x = minRotate_; }

		const Matrix4x4 &mat = Matrix4x4::EulerRotate(rotate_);

		offset = TransformNormal(defaultOffset_, mat);

		viewProjection_.rotation_ = rotate_;

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

bool FollowCamera::ImGuiWidget() {

	if (ImGui::TreeNode("FollowCamera")) {
		bool isUsing = false;

		isUsing |= viewProjection_.ImGuiWidget();

		ImGui::SliderAngle("hSpeed", &cameraRotSpeed_.y, 0.f, 360.f);
		ImGui::SliderAngle("vSpeed", &cameraRotSpeed_.x, 0.f, 360.f);
		ImGui::DragFloat3("defaultOffset", &defaultOffset_.x);
		ImGui::SliderAngle("minRotate", &minRotate_);


		ImGui::TreePop();
		return isUsing;
	}
	return false;

}

const ViewProjection *const FollowCamera::GetViewProjection() const {
	return &viewProjection_;
}
