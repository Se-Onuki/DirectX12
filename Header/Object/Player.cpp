#include "Player.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include <algorithm>
#include <imgui.h>

#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/Math/Math.hpp"
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Utils/SoLib/SoLib_Lerp.h"

#include "LockOn.h"
#include <numeric>
#include "Particle/StarParticle.h"

void Player::ApplyGlobalVariables() {
	const GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *groupName = "Player";
	auto &gGroup = gVariables->GetGroup(groupName);

	transformHead_->translate = gVariables->Get<Vector3>(groupName, "Head Translation");
	transformLeft_->translate = gVariables->Get<Vector3>(groupName, "ArmL Translation");
	transformRight_->translate = gVariables->Get<Vector3>(groupName, "ArmR Translation");

	floatingCycle_ = gVariables->Get<decltype(floatingCycle_)>(groupName, "floatingCycle");
	floatingSwayHand_ = gVariables->Get<decltype(floatingSwayHand_)>(groupName, "floatingSwayHand");
	floatingCycleRange_ =
		gVariables->Get<decltype(floatingCycleRange_)>(groupName, "floatingCycleRange");

	gGroup >> attackClampAngle_;
	attackCycle_ = gVariables->Get<decltype(attackCycle_)>(groupName, "attackCycle");
	gGroup >> attackStartAngle_;
	gGroup >> attackSwingAngle_;

	gGroup >> vDashSpeed_;
	gGroup >> vDashTime_;

	gGroup >> vWeaponCollisionOffset_;
	gGroup >> vWeaponCollisionRadius_;
}

void Player::AddGlobalVariables() const {
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *const groupName = "Player";
	auto &gGroup = gVariables->GetGroup(groupName);

	gVariables->AddValue(groupName, "Head Translation", transformHead_->translate);
	gVariables->AddValue(groupName, "ArmL Translation", transformLeft_->translate);
	gVariables->AddValue(groupName, "ArmR Translation", transformRight_->translate);

	gVariables->AddValue(groupName, "floatingCycle", floatingCycle_);
	gVariables->AddValue(groupName, "floatingSwayHand", floatingSwayHand_);
	gVariables->AddValue(groupName, "floatingCycleRange", floatingCycleRange_);

	gVariables->AddValue(groupName, "attackCycle", attackCycle_);
	gGroup << attackClampAngle_;
	gGroup << attackStartAngle_;
	gGroup << attackSwingAngle_;

	gGroup << vDashSpeed_;
	gGroup << vDashTime_;

	gGroup << vWeaponCollisionOffset_;
	gGroup << vWeaponCollisionRadius_;
}

void Player::InitFloatingGimmick() { floatingParameter_ = 0.f; }

void Player::UpdateFloatingGimmick() {

	const float step = Angle::PI2 / floatingCycle_;

	ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Transform", &transformHead_->translate.x, -10.f, 10.f);
	ImGui::SliderFloat3("Left Transform", &transformLeft_->translate.x, -10.f, 10.f);
	ImGui::SliderFloat3("Right Transform", &transformRight_->translate.x, -10.f, 10.f);
	ImGui::SliderInt("Cycle", &floatingCycle_, 1, 360);
	ImGui::SliderFloat("CycleRange", &floatingCycleRange_, 0.f, 3.f);
	ImGui::SliderAngle("SwayHand", &floatingSwayHand_, 0.f, 360.f);
	ImGui::End();

	floatingParameter_ += step;
	floatingParameter_ = std::fmod(floatingParameter_, Angle::PI2);

	transformBody_->translate.y =
		std::sin(floatingParameter_) * floatingCycleRange_ + originPos;

	transformRight_->rotate.x = std::sin(floatingParameter_) * floatingSwayHand_;
	transformLeft_->rotate.x = std::sin(floatingParameter_) * floatingSwayHand_;
}

void Player::BehaviorRootInit() { floatingParameter_ = 0.f; }

void Player::BehaviorRootUpdate() {

	//XINPUT_STATE inputState{};
	auto vPad = *input_->GetXInput()->GetState();

	static auto *const directInput = input_->GetDirectInput();
	if (directInput->IsAnyPress()) {
		vPad.stickL_ = Vector2::zero();
		if (directInput->IsPress(DIK_W)) {
			++vPad.stickL_.y;
		}
		if (directInput->IsPress(DIK_S)) {
			--vPad.stickL_.y;
		}

		if (directInput->IsPress(DIK_A)) {
			--vPad.stickL_.x;
		}
		if (directInput->IsPress(DIK_D)) {
			++vPad.stickL_.x;
		}
		vPad.stickL_ = vPad.stickL_.Nomalize();
		if (directInput->IsPress(DIK_LSHIFT)) {
			vPad.button_ |= static_cast<WORD>(KeyCode::RIGHT_SHOULDER);
		}
	}

	// 左スティックのデータを受け取る
	Vector3 move{ vPad.stickL_.x, 0.f, vPad.stickL_.y };
	InputRotate(move);


	if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kAttack;
	}
	if (input_->GetXInput()->IsPress(KeyCode::A)) {
		behaviorRequest_ = Behavior::kDash;
	}
	if (input_->GetXInput()->IsPress(KeyCode::B) || input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
	}

	UpdateFloatingGimmick();
	UpdateWorldMatrix();
}

void Player::BehaviorDashInit() {
	floatingParameter_ = 0.f;
	dashTimer_.Start(vDashTime_);
}

void Player::BehaviorDashUpdate(float deltaTime) {
	dashTimer_.Update(deltaTime);
	dashBrake_.Update(deltaTime);
	if (dashTimer_.IsActive()) {
		Vector3 moveVec = Vector3::front;
		Matrix4x4 localMat = transformOrigin_->matWorld_;
		if (transformOrigin_->parent_) {
			localMat *= transformOrigin_->parent_->matWorld_.InverseSRT();
		}
		moveVec = TransformNormal(moveVec, localMat) * vDashSpeed_;
		transformOrigin_->translate += moveVec;
	}
	UpdateWorldMatrix();

	// 時間が切れたら終了
	if (dashTimer_.IsFinish() && dashTimer_.IsActive()) {
		dashBrake_.Start(0.25f);
	}

	if (dashTimer_.IsFinish() && dashBrake_.IsFinish()) {

		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorAttackInit() {
	floatingParameter_ = 0.f;
	workAttack_ = WorkAttack{};

	isAttackActive_ = false;
}

void Player::BehaviorAttackUpdate() {

	//const float step = Angle::PI2 / attackCycle_;

	ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Transform", &transformHead_->translate.x, -10.f, 10.f);
	ImGui::SliderFloat3("Left Transform", &transformLeft_->translate.x, -10.f, 10.f);
	ImGui::SliderFloat3("Right Transform", &transformRight_->translate.x, -10.f, 10.f);
	ImGui::SliderInt("Cycle", &attackCycle_, 1, 360);
	ImGui::SliderAngle("CycleRange", &attackStartAngle_.GetItem());
	ImGui::SliderAngle("SwayHand", &attackSwingAngle_.GetItem());
	ImGui::SliderAngle("ClampAngle", &attackClampAngle_.GetItem());
	ImGui::End();

	if (lockOn_ && lockOn_->IsLockOn()) {

		Vector3 facing = lockOn_->GetTargetPosition() - transformOrigin_->GetGrobalPos();
		facing.y = 0.f;

		if (transformOrigin_->parent_) {
			facing *= transformOrigin_->parent_->matWorld_.GetRotate().InverseRT();
		}

		// ベクトルから回転行列を算出
		Matrix4x4 rotateMat = Matrix4x4::DirectionToDirection(Vector3::front, facing);
		float moveDot = Vector3::front * facing.Nomalize();

		// もし、180度であった場合は調整
		if (moveDot == -1.f) {
			rotateMat = Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, 180._deg);
		}

		transformOrigin_->rotateMat_ = rotateMat;
	}

	const ConstAttack &nowAttack = comboArray_[workAttack_.comboIndex_];
	const AttackTargetAngle &nowAttackAngle = angleArray_[workAttack_.comboIndex_];
	// コンボ上限に達していない場合
	if (workAttack_.comboIndex_ < kComboNum_ - 1u) {
		if (input_->GetXInput()->IsTrigger(KeyCode::RIGHT_SHOULDER)) {
			workAttack_.isComboContinue_ = true;
		}
	}
	// 総時間を経過した場合
	if (++workAttack_.atackParameter_ >= nowAttack.GetTotalTime()) {
		if (workAttack_.isComboContinue_) {
			workAttack_.isComboContinue_ = false;

			auto vPad = *input_->GetXInput()->GetState();
			// 左スティックのデータを受け取る
			Vector3 move{ vPad.stickL_.x, 0.f, vPad.stickL_.y };
			InputRotate(move);

			workAttack_.comboIndex_++;
			workAttack_.atackParameter_ = 0u;
			workAttack_.inComboPhase_ = 0u;
		}
		else {
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	// 現在の状態の進行度
	float progress = (workAttack_.atackParameter_ - static_cast<float>(nowAttack.GetTotalTime(nowAttack.begin() + workAttack_.inComboPhase_))) / *(nowAttack.begin() + workAttack_.inComboPhase_);


	float weaponAngle;
	switch (workAttack_.inComboPhase_) {
	case 0u:
		weaponAngle = SoLib::Lerp(nowAttackAngle.endAngle_, nowAttackAngle.chargeAngle_, progress);

		break;
	case 1u:
		weaponAngle = nowAttackAngle.chargeAngle_;
		break;
	case 2u:
		weaponAngle = SoLib::Lerp(nowAttackAngle.chargeAngle_, nowAttackAngle.siwngAngle_, progress);

		isAttackActive_ = true;

		break;
	case 3u:
		weaponAngle = SoLib::Lerp(nowAttackAngle.siwngAngle_, nowAttackAngle.endAngle_, progress);

		isAttackActive_ = false;

		break;

	default:
		break;
	}


	// 特定の時間に到達したら更新
	if (workAttack_.atackParameter_ >= nowAttack.GetTotalTime(nowAttack.begin() + workAttack_.inComboPhase_ + 1)) {
		// 攻撃パラメータが示している状態を切り替え
		workAttack_.inComboPhase_++;
	}


	//floatingParameter_ += step;
	//if (floatingParameter_ >= Angle::PI2) {
	//	behaviorRequest_ = Behavior::kRoot;
	//}

	//floatingParameter_ = std::fmod(floatingParameter_, Angle::PI2);



	ImGui::SliderAngle("WeaponAngle", &weaponAngle);

	transformWeapon_->rotate.x = weaponAngle;
	transformLeft_->rotate.x = weaponAngle + Angle::PI;
	transformRight_->rotate.x = weaponAngle + Angle::PI;

	weaponColliderViewer_->translate = vWeaponCollisionOffset_;
	weaponColliderViewer_->scale = Vector3::one * vWeaponCollisionRadius_;

	UpdateWorldMatrix();

	weaponCollider_.centor = weaponColliderViewer_->GetGrobalPos();
	weaponCollider_.radius = vWeaponCollisionRadius_;
}

void Player::BehaviorJumpInit() {
	velocity_.y += 100.f;
}

void Player::BehaviorJumpUpdate([[maybe_unused]] float deltaTime) {

	if (transformOrigin_->parent_ && transformOrigin_->translate.y <= 0.f) {
		behaviorRequest_ = Behavior::kRoot;
	}

	UpdateFloatingGimmick();
	UpdateWorldMatrix();
}

void Player::UpdateWorldMatrix() {
	transformOrigin_->UpdateMatrix();

	collider_.SetMatrix(transformOrigin_->matWorld_);

	transformBody_->UpdateMatrix();
	transformHead_->UpdateMatrix();
	transformRight_->UpdateMatrix();
	transformLeft_->UpdateMatrix();

	transformWeapon_->UpdateMatrix();
	weaponColliderViewer_->UpdateMatrix();
}

void Player::InputRotate(const Vector3 &vec) {
	Vector3 move = vec;
	if (move.Length() >= 0.1f) {

		move = move.Nomalize() * moveSpeed_; // 速度を正規化
		Matrix4x4 inputRotateMatrix = Matrix4x4::Identity();
		inputRotateMatrix *= Matrix4x4::Rotate(camera_->matView_.InverseRT());
		//move = TransformNormal(move, camera_->matView_);
		if (transformOrigin_->parent_) {

			//inputRotateMatrix *= Matrix4x4::Rotate(transformOrigin_->parent_->matWorld_.InverseRT());
			//move = TransformNormal(move, transformOrigin_->parent_->matWorld_.InverseSRT());
		}

		move *= inputRotateMatrix;
		velocity_ += move * 5.f; // 移動量を追加

		if (transformOrigin_->parent_) {

			move *= Matrix4x4::Rotate(transformOrigin_->parent_->matWorld_.InverseRT());
			//move = TransformNormal(move, transformOrigin_->parent_->matWorld_.InverseSRT());
		}


		//Vector3 moveCross = Vector3::front.cross(move.Nomalize());
		float moveDot = Vector3::front * move.Nomalize();
		// ベクトルから回転行列を算出
		Matrix4x4 rotateMat = Matrix4x4::DirectionToDirection(Vector3::front, move);

		// もし、180度であった場合は調整
		if (moveDot == -1.f) {
			rotateMat = Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, 180._deg);
		}

		transformOrigin_->rotateMat_ = rotateMat;
	}
}

void Player::Init(const std::unordered_map<std::string, Model *> &model) {

	this->ApplyGlobalVariables();

	BaseCharacter::Init(model);

	// 親子関係
	transformBody_->parent_ = &transformOrigin_;

	transformHead_->parent_ = &transformBody_;
	transformLeft_->parent_ = &transformBody_;
	transformRight_->parent_ = &transformBody_;

	transformWeapon_->parent_ = &transformBody_;

	transformHead_->translate = { 0.f, 2.1f, 0.f };
	transformHead_->rotate += { 0.f, 180._deg, 0.f };
	transformRight_->translate = { +0.75f, 1.5f, 0.f };
	transformRight_->rotate += { 0.f, 180._deg, 0.f };
	transformLeft_->translate = { -0.75f, 1.5f, 0.f };
	transformLeft_->rotate += { 0.f, 180._deg, 0.f };

	transformWeapon_->translate = { 0.f, 2.f, 0.f };
	transformWeapon_->rotate += { 0.f, 180._deg, 0.f };

	weaponColliderViewer_->SetParent(transformWeapon_);


	AddGlobalVariables();

	input_ = Input::GetInstance();
	InitFloatingGimmick();
	collider_.size.y = 5.f;
}

void Player::Update([[maybe_unused]] const float deltaTime) {
	this->ApplyGlobalVariables();

	if (behaviorRequest_) {
		// 振舞いの更新
		behavior_ = behaviorRequest_.value();
		// 振舞いの初期化
		switch (behavior_) {
		case Player::Behavior::kRoot:
			BehaviorRootInit();
			break;
		case Player::Behavior::kDash:
			BehaviorDashInit();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackInit();
			break;
		case Player::Behavior::kJump:
			BehaviorJumpInit();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	velocity_ *= 0.95f;
	velocity_.y += -5.f; // 移動量を追加
	if (transformOrigin_->parent_) {
		transformOrigin_->translate += TransformNormal(velocity_ * deltaTime, transformOrigin_->parent_->matWorld_.InverseRT());
	}
	else {
		transformOrigin_->translate += velocity_ * deltaTime;
	}
	// もし、親コライダよりも
	if (transformOrigin_->parent_ && transformOrigin_->translate.y < 0.f) {
		transformOrigin_->translate.y = 0.f;

		velocity_.y = 0.f;
	}

	if (velocity_.Length() >= 0.5f) {

		auto *particlePtr = ParticleManager::GetInstance()->AddParticle(ModelManager::GetInstance()->GetModel("box"), std::make_unique<StarParticle>(this->GetWorldTransform()->GetGrobalPos()));
		particlePtr->SetAliveTime(1.f);
	}

	switch (behavior_) {
	case Player::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kDash:
		BehaviorDashUpdate(deltaTime);
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Player::Behavior::kJump:
		BehaviorJumpUpdate(deltaTime);
		break;
	}
}

void Player::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	modelMap_.at("body")->Draw(transformBody_, camera);
	modelMap_.at("head")->Draw(transformHead_, camera);
	modelMap_.at("right")->Draw(transformRight_, camera);
	modelMap_.at("left")->Draw(transformLeft_, camera);

	if (behavior_ == Player::Behavior::kAttack) {
		modelMap_.at("weapon")->Draw(transformWeapon_, camera);
	}
}

void Player::SetLockOn(const LockOn *const lockOn) {
	lockOn_ = lockOn;
}

Player::Player() { input_ = Input::GetInstance(); }

Player::~Player() {}

uint32_t Player::ConstAttack::GetTotalTime() const {
	return std::reduce(this->begin(), this->end());
}

uint32_t Player::ConstAttack::GetTotalTime(const uint32_t *end) const
{
	return std::reduce(this->begin(), end);
}
