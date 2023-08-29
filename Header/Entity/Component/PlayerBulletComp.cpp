#include "PlayerBulletComp.h"

#include "Collider.h"
#include "ModelComp.h"

#include "../../Model/ModelManager.h"

void PlayerBulletComp::Init() {
	deathTimer_ = kLifeTime;
	velocity_ = {};
	modelComp_ = object_->AddComponent<ModelComp>();

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();
	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Player));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Player | CollisionFilter::Item)));
	colliderComp->SetRadius(1.5f);
	//colliderComp->SetCentor(Vector3::up() * 1.5f);


	Model *const bulletModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp_->AddBone("body", bulletModel, Transform{ .scale{0.5f,0.5f,5.f} });
}

void PlayerBulletComp::Update() {
	float &modelRotZ = modelComp_->GetBone("body")->transform_.rotate.z;
	modelRotZ = Angle::Mod(modelRotZ + Angle::Dig2Rad * 12.f);

	bool isDead = false;
	isDead |= --deathTimer_ <= 0;	// 寿命を迎えたら死ぬ
	isDead |= object_->transform_.translate.y <= 0.f;	// 地下に潜ったら死ぬ
	if (isDead) {
		object_->SetActive(false);
	}
	Move();
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
