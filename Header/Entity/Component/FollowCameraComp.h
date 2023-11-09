#pragma once
#include "../Entity.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include "../../../Utils/Math/Vector3.h"
#include "../../../Utils/Math/Matrix4x4.h"

class FollowCameraComp : public IComponent {
public:
	using IComponent::IComponent;
	~FollowCameraComp() = default;

	void Update() override;

	void AddRotate(const Vector3 &euler);

	void SetTarget(BaseTransform *const target);

	const auto &GetCamera() const { return camera_; }

private:

	BaseTransform *pTarget_ = nullptr;

	Vector3 offset_{ 0.f,3.f,-15.f };

	Vector3 rotate_;

	Camera3D camera_;
};