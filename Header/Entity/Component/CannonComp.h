#pragma once
#include "../Entity.h"
#include "../../../Engine/DirectBase/File/VariantItem.h"

class CannonComp : public IComponent {
public:
	using IComponent::IComponent;
	~CannonComp() override = default;

	void Init() override;

	void Update() override;

	void SetRider(Entity *const other);

	void Drop();

	void Fire();

	void AddRotate(float angle);

private:

	VariantItem<float> vFirePower_{ "FirePower", 10.f };

	float angle_;

	Entity *pRider_ = nullptr;

};