#include "Player.h"
#include "../../DirectBase/Input/Input.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

void Player::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);
	input_ = Input::GetInstance();
}

void Player::Update() {
	BaseCharacter::Update();
	Vector2 stickL = input_->GetXInput()->GetState()->stickL_;
	Vector3 move = Vector3{ stickL.x, 0, stickL.y } *moveSpeed_;

	if (move.Length() > 0.1f) {

		//move = move.Nomalize() * moveSpeed_; // 速度を正規化
		move =                               // カメラ方向に向けて回転
			move *
			Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y);

		transformOrigin_.translate += move; // 移動量を追加

		const float moveRotate = move.Direction2Euler().y;
		transformOrigin_.rotate.y = Angle::Larp(transformOrigin_.rotate.y, moveRotate, 0.1f);
	}

	transformOrigin_.ImGuiWidget();

	transformOrigin_.UpdateMatrix();
}

void Player::Draw(const ViewProjection &vp) const {
	for (auto &model : modelMap_) {
		model.second->Draw(transformOrigin_, vp);
	}
}
