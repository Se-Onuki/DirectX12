#pragma once
#include "Header/Entity/Entity.h"
#include "Header/Collision/Collision.h"

class StarItemComp : public IComponent {
public:
	using IComponent::IComponent;
	~StarItemComp() = default;

	void Init() override;
	void Reset() override;
	void Update() override;

	void CollectItem();

	const Sphere &GetCollider() const { return collider_; }

	/// @brief 取得されたか
	/// @return 取得されたなら true
	bool GetIsCollected() const { return isCollected_; }

private:

	bool isCollected_ = false;

	Sphere collider_;

};