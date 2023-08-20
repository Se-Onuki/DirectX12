#include "Player.h"
#include "../../DirectBase/Input/Input.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

#include "Component/ModelComp.h"
#include "Component/Collider.h"

void Player::Init() {
	Object::Init();
	input_ = Input::GetInstance();

	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();
}

void Player::Update() {
	Vector2 stickL = input_->GetXInput()->GetState()->stickL_;
	Vector3 move = Vector3{ stickL.x, 0, stickL.y } *moveSpeed_;

	if (move.Length() > 0.1f) {

		//move = move.Nomalize() * moveSpeed_; // 速度を正規化
		move =                               // カメラ方向に向けて回転
			move *
			Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y);

		transform_.translate += move; // 移動量を追加

		const float moveRotate = move.Direction2Euler().y;
		transform_.rotate.y = Angle::Larp(transform_.rotate.y, moveRotate, 0.1f);
	}

	transform_.ImGuiWidget();

	transform_.UpdateMatrix();

	Object::Update();
}

void Player::Draw(const ViewProjection &vp) const {
	Object::Draw(vp);
}

void Player::Attack() {
	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {

		// Vector3 velocity(0, 0, kBulletSpeed);
		// velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		Vector3 velocity = Vector3::front();
		velocity = velocity.Nomalize() * bulletSpeed_;

		/*PlayerBullet *newBullet = new PlayerBullet();
		newBullet->Init(model_, GetPosition(), velocity);

		gameScene_->AddPlayerBullet(newBullet);*/
	}
}
