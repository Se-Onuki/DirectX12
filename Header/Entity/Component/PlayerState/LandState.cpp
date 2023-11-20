#include "LandState.h"
#include "../PlayerComp.h"
#include "IdleState.h"
#include "../Rigidbody.h"

void PlayerLandState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.025f);
}

void PlayerLandState::Update([[maybe_unused]] float deltaTime) {
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();
	rigidbody->SetVelocity({ 0.f,rigidbody->GetVelocity().y,0.f });

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerIdleState>();
	}
}

void PlayerLandState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
