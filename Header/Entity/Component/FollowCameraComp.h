#pragma once
#include "../Entity.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include "../../../Utils/Math/Vector3.h"
#include "../../../Utils/Math/Matrix4x4.h"
#include "../../Collision/Collision.h"
#include "../../../Engine/DirectBase/File/VariantItem.h"

class FollowCameraComp : public IComponent {
public:
	using IComponent::IComponent;
	~FollowCameraComp() = default;

	void Init() override;

	void Update() override;

	void ImGuiWidget() override;

	void AddRotate(const Vector3 &euler);

	void SetTarget(BaseTransform *const target);
	void SetLine(const LineBase &target);

	const auto &GetCamera() const { return camera_; }

	VariantItem<float> vLerpValue{ "LerpValue", 0.75f };

private:

	LineBase line_;

	BaseTransform *pTarget_ = nullptr;

	VariantItem<Vector3> offset_{ "Offset",{ 0.f,6.f,-30.f } };
	VariantItem<Vector3> addOffset_{ "AddOffset",{ 0.f,6.f,0.f } };

	Vector3 rotate_;

	Camera3D *camera_;
};