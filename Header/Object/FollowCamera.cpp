#include "FollowCamera.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Utils/Math/Transform.h"
#include <Xinput.h>
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Utils/SoLib/SoLib_Lerp.h"

void FollowCamera::ApplyGlobalVariables() {
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *const groupName = "FollowCamera";

	gVariables->CreateGroups(groupName);
	auto &gGroup = gVariables->GetGroup(groupName);

	gGroup >> vFollowProgress_;

}

void FollowCamera::AddGlobalVariables() const {
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *const groupName = "FollowCamera";
	auto &gGroup = gVariables->GetGroup(groupName);

	gGroup << vFollowProgress_;
}

void FollowCamera::Init() {
	ApplyGlobalVariables();
	camera_.Init();
	input_ = Input::GetInstance();
	AddGlobalVariables();
}

void FollowCamera::Update() {
	ApplyGlobalVariables();
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
		originPos_ = SoLib::Lerp(originPos_, target_->GetGrobalPos(), vFollowProgress_);

		camera_.translation_ = originPos_ + offset;

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
