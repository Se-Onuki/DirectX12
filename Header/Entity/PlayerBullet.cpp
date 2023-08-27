#include "PlayerBullet.h"

#include "Component/Collider.h"
#include "Component/ModelComp.h"
#include "Component/PlayerBulletComp.h"
#include "Component/Projectile.h"

#include "../Model/ModelManager.h"
#include <unordered_map>
#include <string>

void PlayerBullet::Init() {
	Object::Init();
	AddComponent<PlayerBulletComp>();

	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();

	ProjectileComp *const projectileComp = AddComponent<ProjectileComp>();
	projectileComp->SetDamage(1.f);
}