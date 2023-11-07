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

	void ApplyVariables(const char *const groupName) override;
	void AddVariable(const char *const groupName) const override;

	VariantItem<float> vMoveSpeed{ "MoveSpeed", 1000.f };
	VariantItem<float> vJumpPower{ "JumpPower", 5.f };

private:

	static const std::string groupName_;

	uint32_t registeredGroups_;

	AABB collider_;

	Input *input_;

};