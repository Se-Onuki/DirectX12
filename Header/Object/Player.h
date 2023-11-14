#pragma once

#include "../../Utils/Math/Math.hpp"
#include "../../Engine/DirectBase/Input/Input.h"
#include "../../Utils/Math/Transform.h"
#include "../../Engine/DirectBase/Render/Camera.h"
#include <optional>
#include <stdint.h>

#include "BaseCharacter.h"
#include "../../Engine/DirectBase/File/VariantItem.h"
#include "../Collision/Collision.h"
#include "../../Utils/SoLib/SoLib_Timer.h"


class PlayerBullet;
class Model;

class Player : public BaseCharacter {

	enum class Behavior {
		kRoot,   // 通常状態
		kDash,   // ダッシュ状態
		kAttack, // 攻撃状態
	};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Transform transformBody_;
	Transform transformHead_;
	Transform transformLeft_;
	Transform transformRight_;

	Transform transformWeapon_;

	Transform weaponColliderViewer_;

	Input *input_ = nullptr;
	const Camera<Render::CameraType::Projecction> *camera_ = nullptr;

	const float moveSpeed_ = 0.5f;

	float floatingParameter_ = 0.f;
	const float originPos = 0.5f;

	int32_t floatingCycle_ = 120u;
	float floatingCycleRange_ = 0.2f;
	float floatingSwayHand_ = 30._deg;

	int32_t attackCycle_ = 90u;
	VariantItem<float> attackStartAngle_{ "CycleRange", 0.f * Angle::Dig2Rad };
	VariantItem<float> attackSwingAngle_{ "SwayHand",130.f * Angle::Dig2Rad };
	VariantItem<float> attackClampAngle_{ "ClampAngle",110.f * Angle::Dig2Rad };

	VariantItem<float> vDashSpeed_{ "DashSpeed",4.f };
	VariantItem<float> vDashTime_{ "DashTime",0.075f };

	SoLib::DeltaTimer dashTimer_;

	OBB collider_;

	OBB weaponCollider_;


private:

	void ApplyGlobalVariables();
	void AddGlobalVariables();

	void InitFloatingGimmick();
	void UpdateFloatingGimmick();

	void BehaviorRootInit();
	void BehaviorRootUpdate();

	void BehaviorDashInit();
	void BehaviorDashUpdate(float deltaTime);

	void BehaviorAttackInit();
	void BehaviorAttackUpdate();

	void UpdateWorldMatrix();


public:
	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update(const float deltaTime) override;
	void Draw(const Camera<Render::CameraType::Projecction> &camera) const override;

	void SetCamera(const Camera<Render::CameraType::Projecction> *const camera) { camera_ = camera; }

	const OBB *const GetCollider() const { return &collider_; }
	const OBB *const GetWeaponCollider()const {
		if (behavior_ == Behavior::kAttack) {
			return &weaponCollider_;
		}
		return nullptr;
	}

	Player();
	~Player() override;
};