#include "MoveState.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"

#include "IdleState.h"
#include "FallingState.h"
#include "BeginRotate.h"

// 静的なメンバ変数の実体を宣言
uint32_t PlayerMoveState::stepSE1_ = 0u;
uint32_t PlayerMoveState::stepSE2_ = 0u;

void PlayerMoveState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.1f);

	if (stepSE1_ == 0u) {
		stepSE1_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/step1.wav");
	}
	if (stepSE2_ == 0u) {
		stepSE2_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/step2.wav");
	}

}

void PlayerMoveState::Update([[maybe_unused]] float deltaTime)
{

	static const auto *const keyBoard = input_->GetDirectInput();
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();

	rigidbody->SetVelocity({ 0.f,rigidbody->GetVelocity().y,0.f });

	if (not pPlayer_->GetIsLanding()) {
		pPlayer_->ChangeState<PlayerFallingState>();
	}
	else if (keyBoard->IsPress(DIK_Z)) {
		pPlayer_->ChangeState<PlayerBeginRotateState>();
	}
	else if (keyBoard->IsPress(DIK_SPACE)) {
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

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetAnimationProgress() <= 0.1f) {
		canPlay_ = true;
	}

	if (canPlay_) {
		if (not step1Trigger_) {
			Audio::GetInstance()->PlayWave(stepSE1_, false, 0.45f);
			step1Trigger_ = true;
		}

		// 進捗が半分まで行ったらSEを再生
		if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetAnimationProgress() >= 0.6f) {
			if (not step2Trigger_) {
				Audio::GetInstance()->PlayWave(stepSE2_, false, 0.45f);
				step2Trigger_ = true;
				canPlay_ = false;
			}
		}
	}
	
	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetAnimationProgress() >= 1.0f) {
		step1Trigger_ = false;
		step2Trigger_ = false;
	}
}

void PlayerMoveState::Draw([[maybe_unused]] const Camera3D &camera) const
{
}
