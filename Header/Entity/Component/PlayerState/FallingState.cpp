#include "FallingState.h"
#include "../PlayerComp.h"
#include "LandState.h"
#include "../Rigidbody.h"

void PlayerFallingState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.025f);
}

void PlayerFallingState::Update([[maybe_unused]] float deltaTime) {

	static const auto *const keyBoard = input_->GetDirectInput();
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();

	const float jumpSpeed = pPlayer_->vJumpSpeed_;
	const float jumpDeceleration = pPlayer_->vJumpDeceleration_;
	rigidbody->SetVelocity({ rigidbody->GetVelocity().x * jumpDeceleration,rigidbody->GetVelocity().y,rigidbody->GetVelocity().z * jumpDeceleration });


	Vector3 inputVec{};
	if (keyBoard->IsPress(DIK_W)) {
		inputVec += Vector3::front;
	}
	if (keyBoard->IsPress(DIK_S)) {
		inputVec -= Vector3::front;
	}

	if (keyBoard->IsPress(DIK_A)) {
		inputVec -= Vector3::right;
	}
	if (keyBoard->IsPress(DIK_D)) {
		inputVec += Vector3::right;
	}

	inputVec = inputVec.Nomalize();
	inputVec *= jumpSpeed;

	// 入力方向に応じた移動
	pPlayer_->MoveInput(inputVec);

	if (pPlayer_->GetIsLanding()) {
		pPlayer_->ChangeState<PlayerLandState>();
	}
}

void PlayerFallingState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
