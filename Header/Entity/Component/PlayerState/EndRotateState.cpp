#include "EndRotateState.h"
#include "IdleState.h"
#include "../PlayerComp.h"

void PlayerEndRotateState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
}

void PlayerEndRotateState::Update([[maybe_unused]] float deltaTime) {
	//auto *const groundPosPtr = pPlayer_->GetGroundPos();

	//float targetHeight = groundPosPtr->y + pPlayer_->radius_.y;

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerIdleState>();
	}

}

void PlayerEndRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
