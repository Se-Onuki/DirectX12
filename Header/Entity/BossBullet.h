#pragma once
#include "Object.h"

class BossBullet : public Object {
public:
	BossBullet() = default;
	~BossBullet() override = default;

	void Init() override;

private:
};

class ModelComp;

class BossBulletComp :public IComponent {
public:
	using IComponent::IComponent;
	~BossBulletComp() override = default;

	void Init() override;
	void Update() override;

	void OnCollision(Object *const other) override;

	void Move();

	void SetVelocity(const Vector3 &vec) { velocity_ = vec; }
	void SetPosition(const Vector3 &vec);

	void SetLifeTime(const int32_t time);

private:

	ModelComp *modelComp_ = nullptr;

	int32_t deathTimer_ = 0;
	Vector3 velocity_{};



};