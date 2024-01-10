#pragma once
#include "../Entity.h"

class ProjectileComp :public IComponent {
public:
	using IComponent::IComponent;
	~ProjectileComp() override = default;

	void OnCollision(Entity *other) override;

	void SetDamage(const float value) { damage_ = value; }

private:

	float damage_ = 0.f;
	bool isHitDestroy_ = true;

};