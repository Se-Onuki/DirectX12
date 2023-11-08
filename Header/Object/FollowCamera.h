#pragma once
#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/Math/Math.hpp"
#include "../../Utils/Math/Transform.h"

class Input;

class FollowCamera {
	Camera<Render::CameraType::Projecction> camera_;

	const BaseTransform *target_ = nullptr;
	Vector3 rotate_ = {};
	const float cameraRotSpeed_ = 3.f * Angle::Dig2Rad;
	Input *input_ = nullptr;

public:
	FollowCamera() = default;
	~FollowCamera() = default;

	void Init();
	void Update();
	inline void SetTarget(const BaseTransform *const target) { target_ = target; }

	const Matrix4x4 &GetViewMatrix() const;
	const Matrix4x4 &GetProjectionMatrix() const;

	const auto *const GetCamera() const { return &camera_; }

	bool ImGuiWidget();
};
