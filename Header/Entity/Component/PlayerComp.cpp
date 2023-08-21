#include "PlayerComp.h"
#include "../../../DirectBase/Input/Input.h"
#include "../../../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "Collider.h"
#include "Rigidbody.h"

void PlayerComp::Init() {
	input_ = Input::GetInstance();

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();
	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Player));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Player)));
}

void PlayerComp::Update() {
	Rigidbody *const rigidbody = object_->GetComponent<Rigidbody>();

	Vector2 stickL = input_->GetXInput()->GetState()->stickL_;
	Vector3 move = Vector3{ stickL.x, 0, stickL.y };

	if (move.Length() > 0.1f) {

		//move = move.Nomalize() * moveSpeed_; // 速度を正規化
		move =                               // カメラ方向に向けて回転
			move *
			Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, viewProjection_->rotation_.y) * moveSpeed_;

		rigidbody->AddAcceleration(move); // 移動量を追加

		const float moveRotate = rigidbody->GetVelocity().Direction2Euler().y;
		object_->transform_.rotate.y = Angle::Larp(object_->transform_.rotate.y, moveRotate, 0.15f);
	}

	object_->transform_.ImGuiWidget();

	object_->transform_.CalcMatrix();

}

void PlayerComp::Attack() {
	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {

		Vector3 velocity = Vector3::front();
		velocity = velocity.Nomalize() * bulletSpeed_;

		/*PlayerBullet *newBullet = new PlayerBullet();
		newBullet->Init(model_, GetPosition(), velocity);

		gameScene_->AddPlayerBullet(newBullet);*/
	}
}
