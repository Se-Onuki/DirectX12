#include "PlayerComp.h"
#include "../../../DirectBase/Input/Input.h"
#include "../../../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "ModelComp.h"
#include "../PlayerBullet.h"

#include "../../../Scene/GameScene.h"
#include "PlayerBulletComp.h"
#include <imgui.h>

void PlayerComp::Init() {
	input_ = Input::GetInstance();

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();
	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Player));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Player)));
	colliderComp->SetRadius(10.f);

	modelComp_ = object_->AddComponent<ModelComp>();
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
		object_->transform_.rotate.y = Angle::Lerp(object_->transform_.rotate.y, moveRotate, 0.15f);
	}

	object_->transform_.ImGuiWidget();

	object_->transform_.CalcMatrix();

#pragma region 体の回転

	float &bodyRotateY = modelComp_->GetModel("body")->first.rotate.y;
	const float bodyDiff = viewProjection_->rotation_.y - object_->transform_.rotate.y;
	const float bodyEnd = Angle::Lerp(0.f, bodyDiff, 0.5f);
	bodyRotateY = Angle::Lerp(Angle::Mod(bodyRotateY), bodyEnd, 0.1f);
	ImGui::SliderAngle("bodyRotate", &bodyRotateY);

#pragma endregion

#pragma region 頭の回転

	float &headRotateY = modelComp_->GetModel("head")->first.rotate.y;
	headRotateY = viewProjection_->rotation_.y - object_->transform_.rotate.y;
	ImGui::SliderAngle("headRotate", &headRotateY);

#pragma endregion

	CoolTimeUpdate();

	Attack();

	const Vector3 &velocity = rigidbody->GetVelocity();
	rigidbody->SetVelocity(velocity * friction_);

}

void PlayerComp::Attack() {
	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {
		if (coolTime_ == 0u) {

			AddCoolTime(shotCoolTime_);
			// 弾のベクトルの生成
			Vector3 velocity = Vector3::front();
			velocity = velocity.Nomalize() * bulletSpeed_;
			//const Matrix4x4& rotMat = 
			velocity = TransformNormal(velocity, Matrix4x4::EulerRotate(viewProjection_->rotation_));

			// 弾の生成 + 初期化
			PlayerBullet *newBullet = new PlayerBullet;
			newBullet->Init();
			auto *const bulletComp = newBullet->GetComponent<PlayerBulletComp>();
			bulletComp->SetVelocity(velocity);
			bulletComp->SetPosition(object_->GetWorldPos());

			// 弾の追加
			gameScene_->AddPlayerBullet(newBullet);
		}
	}
}

void PlayerComp::CoolTimeUpdate() {
	if (coolTime_) {
		coolTime_--;
	}
}
