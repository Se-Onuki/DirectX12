#pragma once
#include "../Entity.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../Collision/Collision.h"

#include "../../../Engine/DirectBase/File/VariantItem.h"
#include "FollowCameraComp.h"
#include "../../../Engine/DirectBase/Model/Model.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Update() override;

	void Draw(const Camera3D &camera) const override;

	void ImGuiWidget() override;

	void ApplyVariables(const char *const groupName) override;
	void AddVariable(const char *const groupName) const override;

	VariantItem<float> vMoveSpeed{ "MoveSpeed", 1000.f };
	VariantItem<float> vJumpPower{ "JumpPower", 5.f };

	void SetFollowCamera(FollowCameraComp *const followCamera) { pFollowCamera_ = followCamera; }

private:

	FollowCameraComp *pFollowCamera_ = nullptr;

	static const std::string groupName_;

	// 所属するグループ
	int32_t registeredGroups_ = -1;

	AABB collider_;

	Input *input_;


	Material backMaterial_;
};