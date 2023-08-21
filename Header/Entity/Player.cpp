#include "Player.h"
#include "../../DirectBase/Input/Input.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

#include "Component/ModelComp.h"
#include "Component/Collider.h"
#include "Component/PlayerComp.h"
#include "Component/Rigidbody.h"

void Player::Init() {
	Object::Init();
	AddComponent<Rigidbody>();

	AddComponent<PlayerComp>();

	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();

}

