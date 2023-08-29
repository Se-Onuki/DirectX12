#pragma once

#include "../Object.h"

class ModelComp;

class PlayerBulletComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerBulletComp() = default;

	void Init();
	void Update();

	void OnCollision(Object *const other) override;

	void Move();

	void SetVelocity(const Vector3 &vec) { velocity_ = vec; }
	void SetPosition(const Vector3 &vec);

private:

	ModelComp *modelComp_ = nullptr;

	static const int32_t kLifeTime = 60 * 3;
	int32_t deathTimer_ = kLifeTime;
	Vector3 velocity_{};

};
