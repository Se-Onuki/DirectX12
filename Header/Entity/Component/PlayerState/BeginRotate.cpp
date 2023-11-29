#include "BeginRotate.h"
#include "../PlayerComp.h"
#include "RotatingState.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../../Object/LevelElementManager.h"
#include "../../../Object/TutorialManager.h"

// 静的なメンバ変数の実体を宣言
uint32_t PlayerBeginRotateState::startRotateSE1_ = 0u;
uint32_t PlayerBeginRotateState::startRotateSE2_ = 0u;

void PlayerBeginRotateState::Init() {
	TutorialManager::GetInstance()->SetProgress(TutorialManager::TutorialProgress::kRotate);

	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
	startPos_ = pPlayer_->transform_->translate;

	timer_.Start(pPlayer_->vRotateBeginTime_);

	auto *const platform = LevelElementManager::GetInstance()->GetPlatform(pPlayer_->GetGroup());
	Vector3 cameraFacing = TransformNormal(Vector3::front, CameraManager::GetInstance()->GetUseCamera()->matView_.InverseRT());
	cameraFacing.y = 0.f;
	float rotateFacing = platform->rotateAxis_ * cameraFacing.Nomalize();
	rotateFacing > 0 ? rotateFacing = 1.f : rotateFacing = -1.f;

	auto *const rotateCamera = CameraManager::GetInstance()->AddCamera("RotateCamera");
	pPlayer_->rotateCameraOrigin_ = SoLib::Lerp(platform->center_.translate, pPlayer_->transform_->translate, 0.1f) + Vector3{ 0.f,pPlayer_->vRotateHeight_ - 3.f,0.f };
	// カメラの差分
	Vector3 offsetAngle{ 15._deg, (platform->rotateAxis_ * rotateFacing).Direction2Euler().y ,0.f };

	rotateCamera->translation_ = pPlayer_->rotateCameraOrigin_ + Vector3::front * -45.f * Matrix4x4::EulerRotate(offsetAngle);
	rotateCamera->rotation_ = offsetAngle;
	CameraManager::GetInstance()->GetCameraAnimManager()->Play(rotateCamera, 0.75f, SoLib::easeInOutSine);

	if (startRotateSE1_ == 0u) {
		startRotateSE1_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/startRotate1.wav");
	}
	if (startRotateSE2_ == 0u) {
		startRotateSE2_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/startRotate2.wav");
	}
}

void PlayerBeginRotateState::Update([[maybe_unused]] float deltaTime) {
	int32_t playkey = pAnimation_->GetAnimManager()->GetNowAnimation()->GetPlayKey();
	if (playkey == 5 || playkey == 6 || playkey == 7) {
		timer_.Update(deltaTime);
	}
	float moveLength = pPlayer_->vRotateHeight_ - startPos_.y;
	pPlayer_->transform_->translate = startPos_ + Vector3{ 0.f,moveLength * SoLib::easeInOutQuint(timer_.GetProgress()), 0.f };

	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetIsEnd()) {
		pPlayer_->ChangeState<PlayerRotatingState>();
	}

	if (not start1Trigger_) {
		Audio::GetInstance()->PlayWave(startRotateSE1_, false, 0.45f);
		start1Trigger_ = true;
	}

	// 進捗が半分まで行ったらSEを再生
	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetAnimationProgress() >= 0.2f) {
		if (not start2Trigger_) {
			Audio::GetInstance()->PlayWave(startRotateSE2_, false, 0.45f);
			start2Trigger_ = true;
		}
	}
}

void PlayerBeginRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
