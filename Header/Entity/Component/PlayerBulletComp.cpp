#include "PlayerBulletComp.h"

#include "Collider.h"
#include "ModelComp.h"

#include "../../Model/ModelManager.h"

void PlayerBulletComp::Init() {
	deathTimer_ = kLifeTime;
	velocity_ = {};

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();
	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Player));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Player)));

	ModelComp *const modelComp = object_->AddComponent<ModelComp>();
	Model *const bulletModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp->SetModel({ { "body", { Transform{},bulletModel } } });
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

void PlayerBulletComp::SetPosition(const Vector3 &vec) {
	object_->transform_.translate = vec;
}
