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


class PlayerBullet;
class Model;

class Player : public BaseCharacter {

	enum class Behavior {
		kRoot,   // 通常状態
		kAttack, // 攻撃力
	};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Transform transformBody_;
	Transform transformHead_;
	Transform transformLeft_;
	Transform transformRight_;

	Transform transformWeapon_;

	Input *input_ = nullptr;
	const Camera<Render::CameraType::Projecction> *camera_ = nullptr;

	const float moveSpeed_ = 0.5f;

	float floatingParameter_ = 0.f;
	const float originPos = 0.5f;

	int32_t floatingCycle_ = 120u;
	float floatingCycleRange_ = 0.2f;
	float floatingSwayHand_ = 30 * Angle::Dig2Rad;

	int32_t attackCycle_ = 90u;
	VariantItem<float> attackStartAngle_{ "CycleRange", 0.f * Angle::Dig2Rad };
	VariantItem<float> attackSwingAngle_{ "SwayHand",130.f * Angle::Dig2Rad };
	VariantItem<float> attackClampAngle_{ "ClampAngle",110.f * Angle::Dig2Rad };

	OBB collider_;


private:

	void ApplyClobalVariables();

	void InitFloatingGimmick();
	void UpdateFloatingGimmick();

	void BehaviorRootInit();
	void BehaviorRootUpdate();

	void BehaviorAttackInit();
	void BehaviorAttackUpdate();

	void UpdateWorldMatrix();


public:
	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update(const float deltaTime) override;
	void Draw(const Camera<Render::CameraType::Projecction> &camera) const override;

	void SetCamera(const Camera<Render::CameraType::Projecction> *const camera) { camera_ = camera; }
	const OBB &GetCollider() const { return collider_; }

	Player();
	~Player() override;
};