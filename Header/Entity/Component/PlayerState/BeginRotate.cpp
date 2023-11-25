#include "BeginRotate.h"
#include "../PlayerComp.h"
#include "RotatingState.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../../Object/LevelElementManager.h"

void PlayerBeginRotateState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
	startPos_ = pPlayer_->transform_->translate;

	timer_.Start(pPlayer_->vRotateBeginTime_);

	auto *const platform = LevelElementManager::GetInstance()->GetPlatform(pPlayer_->GetGroup());
	Vector3 cameraFacing = TransformNormal(Vector3::front, CameraManager::GetInstance()->GetUseCamera()->matView_);
	cameraFacing.y = 0.f;
	float rotateFacing = platform->rotateAxis_ * cameraFacing.Nomalize();
	rotateFacing > 0 ? rotateFacing = 1.f : rotateFacing = -1.f;

	auto *const rotateCamera = CameraManager::GetInstance()->AddCamera("RotateCamera");
	rotateCamera->translation_ = SoLib::Lerp(platform->center_.translate, pPlayer_->transform_->translate, 0.1f) + Vector3{ 0.f,pPlayer_->vRotateHeight_ + 5.f,0.f } + platform->rotateAxis_ * rotateFacing * -45.f;
	rotateCamera->rotation_.x = 15._deg;
	rotateCamera->rotation_.y = (platform->rotateAxis_ * rotateFacing).Direction2Euler().y;
	CameraManager::GetInstance()->GetCameraAnimManager()->Play(rotateCamera, 0.75f, SoLib::easeInOutSine);
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
}

void PlayerBeginRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
