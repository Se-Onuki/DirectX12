#include "Player.h"
#include "../../DirectBase/Input/Input.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

#include "Component/ModelComp.h"
#include "Component/Collider.h"
#include "Component/PlayerComp.h"

void Player::Init() {
	Object::Init();
	input_ = Input::GetInstance();

	AddComponent<PlayerComp>();

	AddComponent<ColliderComp>();
	AddComponent<ModelComp>();
}


void Player::Attack() {
	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {

		// Vector3 velocity(0, 0, kBulletSpeed);
		// velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		Vector3 velocity = Vector3::front();
		velocity = velocity.Nomalize() * bulletSpeed_;

		/*PlayerBullet *newBullet = new PlayerBullet();
		newBullet->Init(model_, GetPosition(), velocity);

		gameScene_->AddPlayerBullet(newBullet);*/
	}
}
