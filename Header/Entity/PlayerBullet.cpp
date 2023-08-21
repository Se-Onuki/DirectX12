#include "PlayerBullet.h"

#include "Component/Collider.h"
#include "Component/ModelComp.h"
#include "Component/PlayerBulletComp.h"

#include "../Model/ModelManager.h"
#include <unordered_map>
#include <string>

void PlayerBullet::Init() {
	Object::Init();
	AddComponent<PlayerBulletComp>();

	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();
}