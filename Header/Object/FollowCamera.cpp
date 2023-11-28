#include "FollowCamera.h"
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Utils/Math/Transform.h"
#include "../../Utils/SoLib/SoLib_Lerp.h"
#include "LockOn.h"
#include <Xinput.h>

void FollowCamera::ApplyGlobalVariables()
{
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *const groupName = "FollowCamera";

	gVariables->CreateGroups(groupName);
	auto &gGroup = gVariables->GetGroup(groupName);

	gGroup >> vFollowProgress_;
}

void FollowCamera::AddGlobalVariables() const
{
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *const groupName = "FollowCamera";
	auto &gGroup = gVariables->GetGroup(groupName);

	gGroup << vFollowProgress_;
}

void FollowCamera::Init()
{
	ApplyGlobalVariables();
	camera_.Init();
	input_ = Input::GetInstance();
	AddGlobalVariables();
}

void FollowCamera::Update()
{
	ApplyGlobalVariables();
	if (target_) {
		// ロックオン対象が存在するなら
		if (lockOn_->IsLockOn()) {
			// ロックオン座標
			Vector3 lockOnPos = lockOn_->GetTargetPosition();
			// 追尾対象からロックオン座標へのベクトル
			Vector3 sub = lockOnPos - target_->GetGrobalPos();

			// 縦情報を破棄
			sub.y = 0.f;

			rotateMat_ = Matrix4x4::DirectionToDirection(Vector3::front, sub);
			if (rotateMat_.m[3][3] == 0.f) {
				rotateMat_ = Matrix4x4::Identity();
			}
		}
		else {
			const auto &vPad = input_->GetXInput()->GetState();

			const float rotateStick = vPad->stickR_.x;
			if (std::abs(rotateStick) >= 0.1f) {
				rotateMat_ *= Matrix4x4::EulerRotate(Matrix4x4::Yaw, rotateStick * cameraRotSpeed_);
			}
		}
		Vector3 offset{ 0.f, 3.f, -15.f };
		offset = TransformNormal(offset, rotateMat_);

		camera_.rotateMat_ = rotateMat_;
		originPos_ = SoLib::Lerp(originPos_, target_->GetGrobalPos(), vFollowProgress_);

		camera_.translation_ = originPos_ + offset;
	}
	camera_.UpdateMatrix();
}

void FollowCamera::SetLockOn(const LockOn *const lockOn)
{
	lockOn_ = lockOn;
}

const Matrix4x4 &FollowCamera::GetViewMatrix() const { return camera_.matView_; }

const Matrix4x4 &FollowCamera::GetProjectionMatrix() const { return camera_.matProjection_; }

bool FollowCamera::ImGuiWidget()
{

	if (ImGui::TreeNode("FollowCamera")) {
		bool isUsing = false;

		// isUsing |= ImGui::DragFloat3("Rotate", &rotate_.x, Angle::Dig2Rad);

		ImGui::TreePop();
		return isUsing;
	}
	return false;
}
