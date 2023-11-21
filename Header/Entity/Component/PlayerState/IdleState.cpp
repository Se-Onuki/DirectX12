#include "IdleState.h"
#include "../../../../Engine/DirectBase/Input/Input.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"

#include "MoveState.h"
#include "FallingState.h"


void PlayerIdleState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.15f);
}

void PlayerIdleState::Update([[maybe_unused]] float deltaTime) {

	static const auto *const keyBoard = input_->GetDirectInput();
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();
	rigidbody->SetVelocity({ 0.f,rigidbody->GetVelocity().y,0.f });

	if (not pPlayer_->GetIsLanding()) {
		pPlayer_->ChangeState<PlayerFallingState>();
	}
	else if (keyBoard->IsPress(DIK_SPACE)) {
		pPlayer_->JumpInput();
	}
	else {
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
		if (inputVec.LengthSQ() != 0.f) {
			pPlayer_->ChangeState<PlayerMoveState>();
		}
	}
}

void PlayerIdleState::Draw([[maybe_unused]] const Camera3D &camera) const {

}