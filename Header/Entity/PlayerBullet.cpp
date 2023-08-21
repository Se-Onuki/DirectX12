#include "PlayerBullet.h"
#include "Component/Collider.h"
#include "Component/ModelComp.h"

void PlayerBullet::Init() {
	AddComponent<PlayerBulletComp>();

	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();
}

void PlayerBulletComp::Init() {
	deathTimer_ = kLifeTime;
	velocity_ = {};

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();
	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Player));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Player)));
}

void PlayerBulletComp::Update() {
	Move();
	if (--deathTimer_ <= 0) {
		object_->SetActive(false);
	}
}

void PlayerBulletComp::OnCollision(Object *const) {
	object_->SetActive(false);
}

void PlayerBulletComp::Move() {
	object_->transform_.translate += velocity_;
	object_->transform_.CalcMatrix();
}
