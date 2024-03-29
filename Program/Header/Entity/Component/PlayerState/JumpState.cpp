#include "JumpState.h"
#include "../Rigidbody.h"
#include "../PlayerComp.h"
#include "FallingState.h"

uint32_t PlayerJumpState::jumpSE_ = 0u;

void PlayerJumpState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.025f);
	preAnimKey_ = 0;

	if (jumpSE_ == 0u) {
		jumpSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/jump.wav");
	}
}

void PlayerJumpState::Update([[maybe_unused]] float deltaTime) {

	static const auto *const keyBoard = input_->GetDirectInput();
	static const auto *const gamePad = input_->GetXInput();
	auto *nowAnimation = pAnimation_->GetAnimManager()->GetNowAnimation();
	auto *const rigidbody = pPlayer_->object_->GetComponent<Rigidbody>();

	const float jumpSpeed = pPlayer_->vJumpSpeed_;
	const float jumpDeceleration = pPlayer_->vJumpDeceleration_;
	rigidbody->SetVelocity({ rigidbody->GetVelocity().x * jumpDeceleration,rigidbody->GetVelocity().y,rigidbody->GetVelocity().z * jumpDeceleration });

	if (nowAnimation->GetIsPlay() != 0 && preAnimKey_ == 0) {

		rigidbody->ApplyInstantForce(Vector3{ 0.f,pPlayer_->vJumpPower_,0.f });

	}

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

	Vector3 padInput{};
	padInput.x = gamePad->GetState()->stickL_.x;
	padInput.z = gamePad->GetState()->stickL_.y;
	if (padInput.Length() >= 0.1f) {
		inputVec = padInput;
	}
	inputVec *= jumpSpeed;

	// 入力方向に応じた移動
	pPlayer_->MoveInput(inputVec);

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerFallingState>();
	}
	preAnimKey_ = nowAnimation->GetIsPlay();

	if (not jumpSETrigger_) {
		Audio::GetInstance()->PlayWave(jumpSE_, false, 0.5f);
		jumpSETrigger_ = true;
	}
}

void PlayerJumpState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
