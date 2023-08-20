#include "PlayerComp.h"
#include "../../../DirectBase/Input/Input.h"
#include "../../../DirectBase/3D/ViewProjection/ViewProjection.h"

void PlayerComp::Init() {
	input_ = Input::GetInstance();
}

void PlayerComp::Update() {

	Vector2 stickL = input_->GetXInput()->GetState()->stickL_;
	Vector3 move = Vector3{ stickL.x, 0, stickL.y } *moveSpeed_;

	if (move.Length() > 0.1f) {

		//move = move.Nomalize() * moveSpeed_; // 速度を正規化
		move =                               // カメラ方向に向けて回転
			move *
			Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y);

		object_->transform_.translate += move; // 移動量を追加

		const float moveRotate = move.Direction2Euler().y;
		object_->transform_.rotate.y = Angle::Larp(object_->transform_.rotate.y, moveRotate, 0.1f);
	}

	object_->transform_.ImGuiWidget();

	object_->transform_.CalcMatrix();

}
