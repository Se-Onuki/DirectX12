#include "EndRotateState.h"
#include "IdleState.h"
#include "../PlayerComp.h"

void PlayerEndRotateState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
	timer_.Start(pPlayer_->vRotateBeginTime_);
}

void PlayerEndRotateState::Update([[maybe_unused]] float deltaTime) {
	auto *const groundPosPtr = pPlayer_->GetGroundPos();

	float targetHeight = groundPosPtr->y + pPlayer_->radius_.y;
	SoLib::ImGuiText("GroundPos", std::to_string(targetHeight));
	//int32_t playKey = pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey();

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey() == 0 || pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey() == 1) {
		timer_.Update(deltaTime);
		pPlayer_->transform_->translate.y = SoLib::Lerp(pPlayer_->vRotateHeight_.GetItem(), targetHeight, SoLib::easeInOutSine(timer_.GetProgress()));
	}
	else {
		pPlayer_->transform_->translate.y = pPlayer_->vRotateHeight_;
	}

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerIdleState>();
	}

}

void PlayerEndRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
