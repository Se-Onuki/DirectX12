#include "MoveState.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"

#include "IdleState.h"

void PlayerMoveState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(PlayerBehavior::kMove, true);
}

void PlayerMoveState::Update([[maybe_unused]] float deltaTime)
{

	static const auto *const keyBoard = input_->GetDirectInput();
	//auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();

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

	if (inputVec.LengthSQ() == 0.f) {
		pPlayer_->ChangeState<PlayerIdleState>();
	}

	// 入力方向に応じた移動
	pPlayer_->MoveInput(inputVec);
}

void PlayerMoveState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
