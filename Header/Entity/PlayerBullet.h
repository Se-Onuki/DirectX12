#pragma once
#include "Object.h"

class PlayerBullet : public Object {
public:
	PlayerBullet() = default;
	~PlayerBullet() = default;

	void Init() override;


private:

};

class PlayerBulletComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerBulletComp() = default;

	void Init();
	void Update();

	void OnCollision(Object *const other) override;

	void Move();

private:

	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	Vector3 velocity_;

};
