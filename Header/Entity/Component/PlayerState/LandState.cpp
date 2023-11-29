#include "LandState.h"
#include "../PlayerComp.h"
#include "IdleState.h"
#include "../Rigidbody.h"
#include "MoveState.h"

uint32_t PlayerLandState::landSE_ = 0u;

void PlayerLandState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.01f);

	if (landSE_ == 0u) {
		landSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/land.wav");
	}
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
			(
				/*pAnimation_->GetAnimManager()->GetNowAnimation()->GetFrameProgress() > 0.5f
				||*/
				pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey() >= 1
				)
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

	if (not landSETrigger_) {
		Audio::GetInstance()->PlayWave(landSE_, false, 0.45f);
		landSETrigger_ = true;
	}
}

void PlayerLandState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
