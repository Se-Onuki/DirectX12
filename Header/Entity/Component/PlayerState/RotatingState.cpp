#include "RotatingState.h"
#include "../PlayerComp.h"
#include "EndRotateState.h"

void PlayerRotatingState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.1f);
}

void PlayerRotatingState::Update([[maybe_unused]] float deltaTime) {
	static const auto *const keyBoard = input_->GetDirectInput();
	pPlayer_->transform_->translate.y = pPlayer_->vRotateHeight_;

	auto *const groundPosPtr = pPlayer_->GetGroundPos();
	if (groundPosPtr) {
		if (keyBoard->IsPress(DIK_Z)) {
			pPlayer_->ChangeState<PlayerEndRotateState>();
		}
	}
}

void PlayerRotatingState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
