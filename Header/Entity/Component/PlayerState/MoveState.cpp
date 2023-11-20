#include "MoveState.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"

#include "IdleState.h"

void PlayerMoveState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.1f);
}

void PlayerMoveState::Update([[maybe_unused]] float deltaTime)
{

	static const auto *const keyBoard = input_->GetDirectInput();
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();

	rigidbody->SetVelocity({ 0.f,rigidbody->GetVelocity().y,0.f });

	if (keyBoard->IsPress(DIK_SPACE)) {
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

		inputVec = inputVec.Nomalize();

		// 入力方向に応じた移動
		pPlayer_->MoveInput(inputVec);

		if (inputVec.LengthSQ() == 0.f) {
			pPlayer_->ChangeState<PlayerIdleState>();
		}
	}
}

void PlayerMoveState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
