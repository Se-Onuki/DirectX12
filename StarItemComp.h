#pragma once
#include "Header/Entity/Entity.h"
#include "Header/Collision/Collision.h"

class StarItemComp : public IComponent {
public:
	using IComponent::IComponent;
	~StarItemComp() = default;

	void Init() override;
	void Update() override;

	const Sphere &GetCollider() const { return collider_; }

private:

	Sphere collider_;

};