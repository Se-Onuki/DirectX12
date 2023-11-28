#pragma once
#include "../../Engine/DirectBase/File/VariantItem.h"
#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/Math/Math.hpp"
#include "../../Utils/Math/Transform.h"

class Input;
class LockOn;

class FollowCamera {
	Camera3D camera_;

	const BaseTransform *target_ = nullptr;
	Matrix4x4 rotateMat_ = Matrix4x4::Identity();

	Vector3 originPos_;
	const float cameraRotSpeed_ = 3.f * Angle::Dig2Rad;
	Input *input_ = nullptr;

	const LockOn *lockOn_ = nullptr;

	VariantItem<float> vFollowProgress_{ "FollowProgress", 0.2f };

public:
	FollowCamera() = default;
	~FollowCamera() = default;

	void ApplyGlobalVariables();
	void AddGlobalVariables() const;

	void Init();
	void Update();
	inline void SetTarget(const BaseTransform *const target) { target_ = target; }
	void SetLockOn(const LockOn *const lockOn);

	const Matrix4x4 &GetViewMatrix() const;
	const Matrix4x4 &GetProjectionMatrix() const;

	const auto *const GetCamera() const { return &camera_; }

	bool ImGuiWidget();
};
