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

class LockOn;

class PlayerBullet;
class Model;

class Player : public BaseCharacter {

	enum class Behavior {
		kRoot,		// 通常状態
		kDash,		// ダッシュ状態
		kBrake,		// ブレーキ状態
		kAttack,	// 攻撃状態
		kJump,		// ジャンプ状態
	};

	struct ConstAttack {
		// 予備動作
		uint32_t anticipationTime_;
		// 溜め動作
		uint32_t chargeTime_;
		// 攻撃遷移時間
		uint32_t siwngTime_;
		// 硬直時間
		uint32_t recoveryTime_;

		uint32_t GetTotalTime() const;
		uint32_t GetTotalTime(const uint32_t *end) const;


		const uint32_t *begin() const { return &anticipationTime_; }
		const uint32_t *end()const { return (&recoveryTime_) + 1; }
	};

	struct AttackTargetAngle {
		// 溜め角度
		float chargeAngle_;
		// 攻撃角度
		float siwngAngle_;
		// 終了角度
		float endAngle_{};
	};

	struct WorkAttack {
		// コンボの経過時間
		uint32_t atackParameter_ = 0u;
		// コンボの段数
		int32_t comboIndex_ = 0u;
		// コンボの状態
		int32_t inComboPhase_ = 0u;
		// コンボを継続するか
		bool isComboContinue_ = false;
	};

	static const uint32_t kComboNum_ = 3u;
	std::array<ConstAttack, kComboNum_> comboArray_
	{
		ConstAttack{
			.anticipationTime_ = 3u,
			.chargeTime_ = 3u,
			.siwngTime_ = 3u,
			.recoveryTime_ = 20u,

		},
		ConstAttack{
			.anticipationTime_ = 15u,
			.chargeTime_ = 10u,
			.siwngTime_ = 5u,
			.recoveryTime_ = 30u,

		},
		ConstAttack{
			.anticipationTime_ = 15u,
			.chargeTime_ = 15u,
			.siwngTime_ = 10u,
			.recoveryTime_ = 30u,
		}
	};
	std::array<AttackTargetAngle, kComboNum_> angleArray_
	{
		AttackTargetAngle{
			.chargeAngle_ = 10._deg,
			.siwngAngle_ = -110._deg,

		},
		AttackTargetAngle{
			.chargeAngle_ = 20._deg,
			.siwngAngle_ = -110._deg,

		},
		AttackTargetAngle{
			.chargeAngle_ = 30._deg,
			.siwngAngle_ = -120._deg,
		}
	};
	WorkAttack workAttack_{};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 velocity_{};

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
	VariantItem<float> vDashTime_{ "DashTime",0.75f };

	VariantItem<Vector3> vWeaponCollisionOffset_{ "WeaponCollisionOffset",{} };
	VariantItem<float> vWeaponCollisionRadius_{ "WeaponCollisonRadius",0.75f };

	SoLib::DeltaTimer dashTimer_;
	SoLib::DeltaTimer dashBrake_;

	OBB collider_;

	Sphere weaponCollider_;

	const LockOn *lockOn_ = nullptr;

private:

	void ApplyGlobalVariables();
	void AddGlobalVariables() const;

	void InitFloatingGimmick();
	void UpdateFloatingGimmick();

	void BehaviorRootInit();
	void BehaviorRootUpdate();

	void BehaviorDashInit();
	void BehaviorDashUpdate(float deltaTime);

	void BehaviorAttackInit();
	void BehaviorAttackUpdate();

	void BehaviorJumpInit();
	void BehaviorJumpUpdate(float deltaTime);

	void UpdateWorldMatrix();

	void InputRotate(const Vector3 &vec);


public:
	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update(const float deltaTime) override;
	void Draw(const Camera<Render::CameraType::Projecction> &camera) const override;

	void SetCamera(const Camera<Render::CameraType::Projecction> *const camera) { camera_ = camera; }
	void SetLockOn(const LockOn *const lockOn);

	const OBB *const GetCollider() const { return &collider_; }
	const Sphere *const GetWeaponCollider()const {
		if (behavior_ == Behavior::kAttack) {
			return &weaponCollider_;
		}
		return nullptr;
	}

	const Vector3 &GetVelocity()const { return velocity_; }
	void SetVelocity(const Vector3 &vec) { velocity_ = vec; }

	const Transform &GetWeaponCollisionTransform() const { return weaponColliderViewer_; }

	Player();
	~Player() override;
};