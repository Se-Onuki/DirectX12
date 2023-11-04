#pragma once
#include "../Entity.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../Collision/Collision.h"

#include "../../../Engine/DirectBase/File/VariantItem.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Update() override;

	void Draw(const Camera3D &camera) const override;

	VariantItem<float> vMoveSpeed{ "MoveSpeed", 10.f };

private:

	AABB collider_;

	Input *input_;

};