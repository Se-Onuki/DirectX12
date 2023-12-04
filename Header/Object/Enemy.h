#pragma once

#include "BaseCharacter.h"
#include "../Collision/Collision.h"
#include "Platform.h"
#include "../../Utils/SoLib/SoLib_Timer.h"

class Enemy : public BaseCharacter {
public:
	Enemy();
	~Enemy() override;

	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update(const float deltaTime) override;
	void Draw(const Camera<Render::CameraType::Projecction> &camera) const override;

	void StartDeadAnim(const Vector3 &playerPos);

	void HitWeapon(const Vector3 &playerPos);

	void AddHealth(int32_t value) { health_ += value; }

	const OBB &GetCollider() const { return collider_; }

	void SetPlatform(const Platform *const platform = nullptr);

	uint32_t GetEntityNumber() const { return entityNumber_; }

private:
	int32_t health_ = 3;
	const uint32_t entityNumber_;

	OBB collider_;
	const Platform *pPlatform_ = nullptr;

	static uint32_t sNextNumber_;
};
