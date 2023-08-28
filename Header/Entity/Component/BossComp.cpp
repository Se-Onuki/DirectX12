#include "BossComp.h"
#include "Collider.h"
#include "ModelComp.h"
#include "HealthComp.h"
#include "../../Model/ModelManager.h"

void BossComp::Init() {

	auto *const colliderComp = object_->AddComponent<ColliderComp>();
	auto *const modelComp = object_->AddComponent<ModelComp>();
	auto *const healthComp = object_->AddComponent<HealthComp>();

	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Enemy));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Enemy)));
	colliderComp->SetRadius(3.f);
	colliderComp->SetCentor(Vector3::up() * 3.f);

	Model *const enemyModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp->AddBone("body", enemyModel, Transform{ .translate{0.f,3.f,0.f} });

	healthComp->SetMaxHealth(100.f);
	healthComp->Reset();
}
