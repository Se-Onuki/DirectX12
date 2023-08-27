#include "Enemy.h"
#include "Component/Collider.h"
#include "Component/ModelComp.h"

#include "../Model/ModelManager.h"
#include "Component/HealthComp.h"

void Enemy::Init() {
	Object::Init();
	auto *const colliderComp = AddComponent<ColliderComp>();
	auto *const modelComp = AddComponent<ModelComp>();
	auto *const healthComp = AddComponent<HealthComp>();

	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Enemy));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Enemy)));
	colliderComp->SetRadius(3.f);
	//colliderComp->SetCentor(Vector3::up() * 3.f);

	Model *const enemyModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp->SetModel(
		{
			{"body",{Transform{}, enemyModel}},
		}
	);

	healthComp->SetMaxHealth(3.f);
	healthComp->Reset();
}
