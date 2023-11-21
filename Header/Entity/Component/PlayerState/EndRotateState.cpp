#include "EndRotateState.h"
#include "IdleState.h"
#include "../PlayerComp.h"

void PlayerEndRotateState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
	timer_.Start(pPlayer_->vRotateEndTime_);
}

void PlayerEndRotateState::Update([[maybe_unused]] float deltaTime) {
	auto *const groundPosPtr = pPlayer_->GetGroundPos();

	float targetHeight = groundPosPtr->y + pPlayer_->radius_.y;
	SoLib::ImGuiText("GroundPos", std::to_string(targetHeight));

	timer_.Update(deltaTime);
	pPlayer_->transform_->translate.y = SoLib::Lerp(pPlayer_->vRotateHeight_.GetItem(), targetHeight, SoLib::easeInOutQuad(timer_.GetProgress()));

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerIdleState>();
	}

}

void PlayerEndRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
