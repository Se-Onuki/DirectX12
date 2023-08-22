#pragma once
#include <list>
//#include "Collision.h"

#include "../Math/Vector2.h"

class Object;
class ColliderComp;
class ViewProjection;

class Targeting {
	std::list<ColliderComp *> colliderList_{};

	Vector2 reticlePos_{};
	//LineBase mouseSegment_ = {};

	ColliderComp *hitCollider_ = nullptr;

	float radius_ = 150.f;

	Targeting() = default;
	Targeting(const Targeting &) = delete;
	Targeting operator=(const Targeting &) = delete;
	~Targeting() = default;

public:
	static Targeting *GetInstance() {
		static Targeting Targeting;
		return &Targeting;
	}

	void clear();
	void push_back(ColliderComp *collider) { colliderList_.push_back(collider); }
	void push_back(Object *object);
	void SetPosition(const Vector2 &newPos) { reticlePos_ = newPos; }
	void SetRadius(const float rad) { radius_ = rad; }

	ColliderComp *const GetHitCollider() const { return hitCollider_; }

	void Update(const ViewProjection &vp);
};