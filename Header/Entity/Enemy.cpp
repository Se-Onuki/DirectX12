#include "Enemy.h"
#include "Component/Collider.h"
#include "Component/ModelComp.h"

#include "../Model/ModelManager.h"

void Enemy::Init() {

	auto *const colliderComp = AddComponent<ColliderComp>();
	auto *const modelComp = AddComponent<ModelComp>();

	colliderComp->SetCollisionAttribute(static_cast<uint32_t>(CollisionFilter::Enemy));
	colliderComp->SetCollisionMask(~(static_cast<uint32_t>(CollisionFilter::Enemy)));
	colliderComp->SetRadius(3.f);

	Model *const enemyModel = ModelManager::GetInstance()->GetModel("sphere");
	modelComp->SetModel(
		{
			{"body",{Transform{}, enemyModel}},
		}
	);
}
