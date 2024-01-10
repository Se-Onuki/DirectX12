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

	auto *const platform = LevelElementManager::GetInstance()->GetPlatform(pPlayer_->GetGroup());
	Vector3 cameraFacing = TransformNormal(Vector3::front, CameraManager::GetInstance()->GetUseCamera()->matView_.InverseRT());
	cameraFacing.y = 0.f;
	rotateFacing_ = platform->rotateAxis_ * cameraFacing.Nomalize();
	rotateFacing_ > 0 ? rotateFacing_ = 1.f : rotateFacing_ = -1.f;

	rotateCamera_ = CameraManager::GetInstance()->AddCamera("RotateCamera");
	// カメラの差分
	neutralAngle_ = Vector3{ 15._deg, (platform->rotateAxis_ * rotateFacing_).Direction2Euler().y ,0.f };

}

void PlayerRotatingState::Update([[maybe_unused]] float deltaTime) {
	static const auto *const keyBoard = input_->GetDirectInput();
	static const auto *const gamePad = input_->GetXInput();
	pPlayer_->transform_->translate.y = pPlayer_->vRotateHeight_;

	auto *const groundPosPtr = pPlayer_->GetGroundPos();

	auto *const platform = LevelElementManager::GetInstance()->GetPlatform(pPlayer_->GetGroup());

	addAngle_ = SoLib::Lerp(Vector3::zero, addAngle_, 0.96f);

	rotateCamera_->translation_ = pPlayer_->rotateCameraOrigin_ + Vector3::front * -45.f * Matrix4x4::EulerRotate(neutralAngle_ + addAngle_);
	rotateCamera_->rotation_ = neutralAngle_ + addAngle_;

	const Vector3 *downPtr = pPlayer_->GetGroundPos();
	Vector3 downPlayer;
	if (downPtr) {
		endColor_ = { 0.1f,0.1f,1.f,1.f };
		downPlayer = *downPtr;
	}
	else {
		endColor_ = { 1.f,0.1f,0.1f,1.f };

		downPlayer = pPlayer_->transform_->GetGrobalPos();
		downPlayer.y -= 45.f;
	}
	if (platform->GetTimer().IsActive()) {
		downPlayer.y -= 45.f;
	}


	startColor_ = SoLib::Lerp(startColor_, endColor_, 0.2f);
	for (uint32_t i = 0u; i < 2.f; i++) {
		TestParticle *particlePtr = dynamic_cast<TestParticle *>(ParticleManager::GetInstance()->AddParticle(model_, std::make_unique<TestParticle>(SoLib::Lerp(pPlayer_->transform_->GetGrobalPos(), downPlayer, Random::GetRandom<float>(0.f, 1.f)))));
		if (particlePtr) {

			particlePtr->velocity_ = Vector3::front * Random::GetRandom<float>(0.5f, 2.f) * Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, Random::GetRandom<float>(-Angle::PI, Angle::PI));
			particlePtr->color_ = startColor_;
			particlePtr->SetAliveTime(0.5f);
			particlePtr->endScale_ = Vector3::one * 0.5f;

		}
	}

	if (platform->GetTimer().IsFinish()) {
		if (keyBoard->IsPress(DIK_Q) || gamePad->IsPress(KeyCode::LEFT_SHOULDER)) {
			Audio::GetInstance()->PlayWave(rotateSE_, false, 0.45f);
			platform->AddRotate(rotateFacing_ * 90._deg);
		}
		else if (keyBoard->IsPress(DIK_E) || gamePad->IsPress(KeyCode::RIGHT_SHOULDER)) {
			Audio::GetInstance()->PlayWave(rotateSE_, false, 0.45f);
			platform->AddRotate(rotateFacing_ * -90._deg);
		}
		else if (groundPosPtr && (keyBoard->IsPress(DIK_Z) || gamePad->IsPress(KeyCode::X))) {
			pPlayer_->ChangeState<PlayerEndRotateState>();
		}
	}


}

void PlayerRotatingState::Draw([[maybe_unused]] const Camera3D &camera) const {
}

void PlayerRotatingState::AddRotate(const Vector3 &angle) {
	addAngle_ += angle;
}
