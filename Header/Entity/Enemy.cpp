#include "Enemy.h"
#include "Component/Collider.h"
#include "Component/ModelComp.h"

#include "../Model/ModelManager.h"
#include "Component/HealthComp.h"
#include "Component/BossComp.h"

void Enemy::Init() {
	Object::Init();
	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();
	AddComponent<HealthComp>();
	AddComponent<BossComp>();
}
