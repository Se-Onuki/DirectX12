#include "BeginRotate.h"
#include "../PlayerComp.h"

void PlayerBeginRotateState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
	startPos_ = pPlayer_->transform_->translate;

	timer_.Start(pPlayer_->vRotateBeginTime_);
}

void PlayerBeginRotateState::Update([[maybe_unused]] float deltaTime) {
	int32_t playkey = pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey();
	if (playkey == 5 || playkey == 6 || playkey == 7) {
		timer_.Update(deltaTime);
	}
	float moveLength = pPlayer_->vRotateHeight_ - startPos_.y;
	pPlayer_->transform_->translate = startPos_ + Vector3{ 0.f,moveLength * SoLib::easeInOutQuint(timer_.GetProgress()), 0.f };
}

void PlayerBeginRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
