#pragma once

#include "BaseCharacter.h"
#include "../Collision/Collision.h"
#include "Platform.h"

class Enemy : public BaseCharacter {
public:
	Enemy() = default;
	~Enemy() override;

	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update(const float deltaTime) override;
	void Draw(const Camera<Render::CameraType::Projecction> &camera) const override;

	const OBB &GetCollider() const { return collider_; }

	void SetPlatform(const Platform *const platform = nullptr);

private:
	OBB collider_;
	const Platform *pPlatform_ = nullptr;
};
