#include "EndRotateState.h"
#include "IdleState.h"
#include "../PlayerComp.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../../Object/TutorialManager.h"
#include "../../Header/Object/InGameUIManager/InGameUIManager.h"
#include "../../Header/Object/LevelElementManager.h"
#include "../../../../StarItemComp.h"


// 静的なメンバ変数の実体を宣言
uint32_t PlayerEndRotateState::endRotateSE1_ = 0u;
uint32_t PlayerEndRotateState::endRotateSE2_ = 0u;

void PlayerEndRotateState::Init() {
	static auto *const tutorial = TutorialManager::GetInstance();

	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), false, AnimEasing::kLinear, 0.1f);
	timer_.Start(pPlayer_->vRotateEndTime_);

	auto *const followCamera = CameraManager::GetInstance()->AddCamera("FollowCamera");

	CameraManager::GetInstance()->GetCameraAnimManager()->Play(followCamera, 0.75f, SoLib::easeInOutSine);

	if (endRotateSE1_ == 0u) {
		endRotateSE1_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/endRotate1.wav");
	}
	if (endRotateSE2_ == 0u) {
		endRotateSE2_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/land.wav");
	}

	bool isNotClear = false;

	for (const auto &item : LevelElementManager::GetInstance()->GetStarItemList()) {
		isNotClear |= not item->GetComponent<StarItemComp>()->GetIsCollected();
		if (isNotClear) {
			break;
		}
	}

	// もし回転が行われていたら減少
	if (LevelElementManager::GetInstance()->GetStarCount() == 0) {
		tutorial->SetProgress(TutorialManager::TutorialProgress::kWalk);
	}
	else if (pPlayer_->rotateCount_ % 4 != 0) {
		tutorial->SetProgress(TutorialManager::TutorialProgress::kToGoal);
	}
	else {
		tutorial->SetProgress(TutorialManager::TutorialProgress::kFloatZ);
	}
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

	if (not end1Trigger_) {
		Audio::GetInstance()->PlayWave(endRotateSE1_, false, 0.45f);
		end1Trigger_ = true;
	}

	// 進捗が半分まで行ったらSEを再生
	if (pAnimation_->GetAnimManager()->GetNowAnimation()->GetAnimationProgress() >= 0.35f) {
		if (not end2Trigger_) {
			Audio::GetInstance()->PlayWave(endRotateSE2_, false, 0.45f);
			end2Trigger_ = true;
		}
	}
}

void PlayerEndRotateState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
