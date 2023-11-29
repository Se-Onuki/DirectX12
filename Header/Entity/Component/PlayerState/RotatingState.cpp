#include "RotatingState.h"
#include "../PlayerComp.h"
#include "EndRotateState.h"
#include "../../../Object/LevelElementManager.h"
#include "../../../../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"
#include "../../../../Engine/DirectBase/Model/ModelManager.h"
#include "../../../Object/Particle/ParticleManager.h"
#include "../../../Object/Particle/TestParticle.h"

uint32_t PlayerRotatingState::rotateSE_ = 0u;

void PlayerRotatingState::Init() {
	pAnimation_->GetAnimManager()->SetNextAnimation(GetState(), true, AnimEasing::kLinear, 0.1f);

	rotateCamera_ = CameraManager::GetInstance()->AddCamera("RotateCamera");


	if (rotateSE_ == 0u) {
		rotateSE_ = Audio::GetInstance()->LoadWave("resources/Audio/SE/Player/rotate.wav");
	}

	model_ = ModelManager::GetInstance()->GetModel("DownBox");
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

	const Vector3 *downPtr = pPlayer_->GetGroundPos();
	Vector3 downPlayer;
	if (downPtr) {
		endColor_ = { 0.3f,0.3f,1.f,1.f };
		downPlayer = *downPtr;
	}
	else {
		endColor_ = { 1.f,0.3f,0.3f,1.f };

		downPlayer = pPlayer_->transform_->GetGrobalPos();
		downPlayer.y -= 15.f;
	}
	startColor_ = SoLib::Lerp(startColor_, endColor_, 0.1f);
	for (uint32_t i = 0u; i < 2.f; i++) {
		TestParticle *particlePtr = dynamic_cast<TestParticle *>(ParticleManager::GetInstance()->AddParticle(model_, std::make_unique<TestParticle>(SoLib::Lerp(pPlayer_->transform_->GetGrobalPos(), downPlayer, Random::GetRandom<float>(0.f, 1.f)))));
		if (particlePtr) {

			particlePtr->velocity_ = Vector3::front * Random::GetRandom<float>(2.f, 5.f) * Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, Random::GetRandom<float>(-Angle::PI, Angle::PI));
			particlePtr->color_ = startColor_;
			particlePtr->SetAliveTime(0.25f);

		}
	}

	if (platform->GetTimer().IsFinish()) {
		if (keyBoard->IsPress(DIK_Q) || gamePad->IsPress(KeyCode::LEFT_SHOULDER)) {
			Audio::GetInstance()->PlayWave(rotateSE_, false, 0.45f);
			platform->AddRotate(rotateFacing * 90._deg);
		}
		else if (keyBoard->IsPress(DIK_E) || gamePad->IsPress(KeyCode::RIGHT_SHOULDER)) {
			Audio::GetInstance()->PlayWave(rotateSE_, false, 0.45f);
			platform->AddRotate(rotateFacing * -90._deg);
		}
		else if (groundPosPtr && (keyBoard->IsPress(DIK_Z) || gamePad->IsPress(KeyCode::X))) {
			pPlayer_->ChangeState<PlayerEndRotateState>();
		}
	}


}

void PlayerRotatingState::Draw([[maybe_unused]] const Camera3D &camera) const {
}
