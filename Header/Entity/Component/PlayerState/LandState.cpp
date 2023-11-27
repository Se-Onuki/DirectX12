#include "LandState.h"
#include "../PlayerComp.h"
#include "IdleState.h"
#include "../Rigidbody.h"
#include "MoveState.h"

void PlayerLandState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.01f);
}

void PlayerLandState::Update([[maybe_unused]] float deltaTime) {
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();
	rigidbody->SetVelocity({ 0.f,rigidbody->GetVelocity().y,0.f });

	static auto *const keyboard = input_->GetDirectInput();

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerIdleState>();
	}
	else if
		(
			pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey() >= 1
			&& (
				input_->GetXInput()->GetState()->stickL_.Length() > 0.1f
				|| (
					keyboard->IsPress(DIK_W)
					|| keyboard->IsPress(DIK_S)
					|| keyboard->IsPress(DIK_A)
					|| keyboard->IsPress(DIK_D)
					)
				)
			) {
		pPlayer_->ChangeState<PlayerMoveState>();
	}
}

void PlayerLandState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
