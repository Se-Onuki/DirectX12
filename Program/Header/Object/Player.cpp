#include "Player.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include <algorithm>
#include <imgui.h>

#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/Math/Math.hpp"
#include "../../Engine/DirectBase/File/GlobalVariables.h"
#include "../../Utils/SoLib/SoLib_Lerp.h"

void Player::ApplyClobalVariables() {
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	gVariables;
	const char *groupName = "Player";
	groupName;
	transformHead_->translate = gVariables->Get<Vector3>(groupName, "Head Translation");
	transformLeft_->translate = gVariables->Get<Vector3>(groupName, "ArmL Translation");
	transformRight_->translate = gVariables->Get<Vector3>(groupName, "ArmR Translation");

	floatingCycle_ = gVariables->Get<decltype(floatingCycle_)>(groupName, "floatingCycle");
	floatingSwayHand_ = gVariables->Get<decltype(floatingSwayHand_)>(groupName, "floatingSwayHand");
	floatingCycleRange_ =
		gVariables->Get<decltype(floatingCycleRange_)>(groupName, "floatingCycleRange");

	attackClampAngle_ = gVariables->Get<float>(groupName, "attackClampAngle");
	attackCycle_ = gVariables->Get<decltype(attackCycle_)>(groupName, "attackCycle");
	attackStartAngle_ = gVariables->Get<float>(groupName, "attackStartAngle");
	attackSwingAngle_ = gVariables->Get<float>(groupName, "attackSwingAngle");
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
	//if (input_->GetJoystickState(0, inputState)) {
	const auto &vPad = input_->GetXInput()->GetState();

	// 左スティックのデータを受け取る
	Vector3 move{ vPad->stickL_.x, 0.f, vPad->stickL_.y };
	if (move != Vector3::zero) {

		move = move.Nomalize() * moveSpeed_; // 速度を正規化
		move =                               // カメラ方向に向けて回転
			move *
			Matrix4x4::EulerRotate(Matrix4x4::EulerAngle::Yaw, camera_->rotation_.y);

		transformOrigin_->translate += move; // 移動量を追加

		transformOrigin_->rotate = Quaternion::LookAt(move); // ベクトルからオイラー角を算出
	}

	if (input_->GetXInput()->IsPress(KeyCode::RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kAttack;
	}
	//}

	UpdateFloatingGimmick();
	UpdateWorldMatrix();
}

void Player::BehaviorAttackInit() { floatingParameter_ = 0.f; }

void Player::BehaviorAttackUpdate() {

	const float step = Angle::PI2 / attackCycle_;

	ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Transform", &transformHead_->translate.x, -10.f, 10.f);
	ImGui::SliderFloat3("Left Transform", &transformLeft_->translate.x, -10.f, 10.f);
	ImGui::SliderFloat3("Right Transform", &transformRight_->translate.x, -10.f, 10.f);
	ImGui::SliderInt("Cycle", &attackCycle_, 1, 360);
	ImGui::SliderAngle("CycleRange", &attackStartAngle_.GetItem());
	ImGui::SliderAngle("SwayHand", &attackSwingAngle_.GetItem());
	ImGui::SliderAngle("ClampAngle", &attackClampAngle_.GetItem());
	ImGui::End();

	floatingParameter_ += step;
	if (floatingParameter_ >= Angle::PI2) {
		behaviorRequest_ = Behavior::kRoot;
	}
	floatingParameter_ = std::fmod(floatingParameter_, Angle::PI2);

	transformWeapon_->rotate.x = std::clamp<float>(
		std::sin(floatingParameter_) * attackSwingAngle_ + attackStartAngle_, 0.f,
		attackClampAngle_);
	transformLeft_->rotate.x =
		std::clamp<float>(
			std::sin(floatingParameter_) * attackSwingAngle_ + attackStartAngle_, 0.f,
			attackClampAngle_) +
		Angle::PI;
	transformRight_->rotate.x =
		std::clamp<float>(
			std::sin(floatingParameter_) * attackSwingAngle_ + attackStartAngle_, 0.f,
			attackClampAngle_) +
		Angle::PI;

	UpdateWorldMatrix();
}

void Player::UpdateWorldMatrix() {
	transformOrigin_->UpdateMatrix();

	transformBody_->UpdateMatrix();
	transformHead_->UpdateMatrix();
	transformRight_->UpdateMatrix();
	transformLeft_->UpdateMatrix();

	transformWeapon_->UpdateMatrix();
}

void Player::Init(const std::unordered_map<std::string, Model *> &model) {
	GlobalVariables *const gVariables = GlobalVariables::GetInstance();
	const char *const groupName = "Player";
	ApplyClobalVariables();

	BaseCharacter::Init(model);

	// メモリ確保

	// 親子関係
	transformBody_->parent_ = &transformOrigin_;

	transformHead_->parent_ = &transformBody_;
	transformLeft_->parent_ = &transformBody_;
	transformRight_->parent_ = &transformBody_;

	transformWeapon_->parent_ = &transformBody_;

	transformHead_->translate = { 0.f, 2.1f, 0.f };
	transformRight_->translate = { +0.75f, 1.5f, 0.f };
	transformLeft_->translate = { -0.75f, 1.5f, 0.f };

	transformWeapon_->translate = { 0.f, 2.f, 0.f };

	gVariables->AddValue(groupName, "Head Translation", transformHead_->translate);
	gVariables->AddValue(groupName, "ArmL Translation", transformLeft_->translate);
	gVariables->AddValue(groupName, "ArmR Translation", transformRight_->translate);

	gVariables->AddValue(groupName, "floatingCycle", floatingCycle_);
	gVariables->AddValue(groupName, "floatingSwayHand", floatingSwayHand_);
	gVariables->AddValue(groupName, "floatingCycleRange", floatingCycleRange_);

	gVariables->AddValue(groupName, "attackCycle", attackCycle_);
	gVariables->AddValue(groupName, attackClampAngle_);
	gVariables->AddValue(groupName, attackStartAngle_);
	gVariables->AddValue(groupName, attackSwingAngle_);

	input_ = Input::GetInstance();
	InitFloatingGimmick();
}

void Player::Update() {
	if (behaviorRequest_) {
		// 振舞いの更新
		behavior_ = behaviorRequest_.value();
		// 振舞いの初期化
		switch (behavior_) {
		case Player::Behavior::kRoot:
			BehaviorRootInit();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackInit();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Player::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}
	if (input_->GetDirectInput()->IsTrigger(DIK_R)) {
		ApplyClobalVariables();
	}
}

void Player::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	modelMap_.at("body")->Draw(transformBody_, camera);
	modelMap_.at("head")->Draw(transformHead_, camera);
	modelMap_.at("right")->Draw(transformRight_, camera);
	modelMap_.at("left")->Draw(transformLeft_, camera);

	if (behavior_ == Player::Behavior::kAttack)
		modelMap_.at("weapon")->Draw(transformWeapon_, camera);
}

Player::Player() { input_ = Input::GetInstance(); }

Player::~Player() {}
