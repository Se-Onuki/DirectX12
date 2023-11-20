#include "IdleState.h"
#include "../../../../Engine/DirectBase/Input/Input.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"

#include "MoveState.h"


void PlayerIdleState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true);
}

void PlayerIdleState::Update([[maybe_unused]] float deltaTime) {

	static const auto *const keyBoard = input_->GetDirectInput();
	//auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();


	if (keyBoard->IsPress(DIK_W) || keyBoard->IsPress(DIK_S) || keyBoard->IsPress(DIK_A) || keyBoard->IsPress(DIK_D)) {
		pPlayer_->ChangeState<PlayerMoveState>();
	}

}

void PlayerIdleState::Draw([[maybe_unused]] const Camera3D &camera) const {

}