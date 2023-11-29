#include "RotatingState.h"
#include "../PlayerComp.h"
#include "EndRotateState.h"
#include "../../../Object/LevelElementManager.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

uint32_t PlayerRotatingState::rotateSE_ = 0u;

void PlayerRotatingState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.1f);

	rotateCamera_ = CameraManager::GetInstance()->AddCamera("RotateCamera");


	if (rotateSE_ == 0u) {
		rotateSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/rotate.wav");
	}
}

void PlayerRotatingState::Update([[maybe_unused]] float deltaTime) {
	static const auto *const keyBoard = input_->GetDirectInput();
	static const auto *const gamePad = input_->GetXInput();
	pPlayer_->transform_->translate.y = pPlayer_->vRotateHeight_;

	auto *const groundPosPtr = pPlayer_->GetGroundPos();

	auto *const platform = LevelElementManager::GetInstance()->GetPlatform(pPlayer_->GetGroup());
	Vector3 cameraFacing = TransformNormal(Vector3::front, CameraManager::GetInstance()->GetUseCamera()->matView_.InverseRT());
	cameraFacing.y = 0.f;
	float rotateFacing = platform->rotateAxis_ * cameraFacing.Nomalize();

	rotateFacing > 0 ? rotateFacing = 1.f : rotateFacing = -1.f;

	if (platform->GetTimer().IsFinish()) {
		if (keyBoard->IsPress(DIK_Q) || gamePad->IsPress(KeyCode::LEFT_SHOULDER)) {
			Audio::GetInstance()->PlayWave(rotateSE_, false, 0.45f);
			platform->AddRotate(rotateFacing * 90._deg);
		}
		else if (keyBoard->IsPress(DIK_E) || gamePad->IsPress(KeyCode::RIGHT_SHOULDER)) {
			Audio::GetInstance()->PlayWave(rotateSE_, false, 0.45f);
			platform->AddRotate(rotateFacing * -90._deg);
		}
		else if (groundPosPtr && (keyBoard->IsPress(DIK_Z) || gamePad->IsPress(KeyCode::Y))) {
			pPlayer_->ChangeState<PlayerEndRotateState>();
		}
	}


}

void PlayerRotatingState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
