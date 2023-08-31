#include "BossBullet.h"
#include "Component/Collider.h"
#include "Component/ModelComp.h"
#include "Component/HealthComp.h"

#include "../Model/ModelManager.h"

void BossBullet::Init() {
	Object::Init();

	AddComponent<BossBulletComp>();
}



void BossBulletComp::Init() {
	deathTimer_ = kLifeTime;
	velocity_ = {};
	modelComp_ = object_->AddComponent<ModelComp>();

	ColliderComp *const colliderComp = object_->AddComponent<ColliderComp>();

	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Enemy | CollisionFilter::Bullet));	// 自分の属性
	colliderComp->SetCollisionMask(~static_cast<uint32_t>(CollisionFilter::Enemy | CollisionFilter::Bullet));		// 検知する属性

	colliderComp->SetCollisionCancel(static_cast<uint32_t>(CollisionFilter::Enemy | CollisionFilter::Bullet));		// 無効化する属性
	colliderComp->SetRadius(1.5f);
	//colliderComp->SetCentor(Vector3::up() * 1.5f);


	Model *const bulletModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp_->AddBone("body", bulletModel, Transform{ .scale{0.5f,0.5f,5.f} });
}

void BossBulletComp::Update() {
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

void BossBulletComp::OnCollision(Object *const) {
	object_->SetActive(false);
}

void BossBulletComp::Move() {
	object_->transform_.translate += velocity_;
	object_->transform_.CalcMatrix();
}

void BossBulletComp::SetPosition(const Vector3 &vec) {
	object_->transform_.translate = vec;
}
