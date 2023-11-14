#pragma once
#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/Math/Math.hpp"
#include "../../Utils/Math/Transform.h"
#include "../../Engine/DirectBase/File/VariantItem.h"

class Input;

class FollowCamera {
	Camera3D camera_;

	const BaseTransform *target_ = nullptr;
	Vector3 rotate_ = {};
	Vector3 originPos_;
	const float cameraRotSpeed_ = 3.f * Angle::Dig2Rad;
	Input *input_ = nullptr;

	VariantItem<float> vFollowProgress_{ "FollowProgress", 0.2f };

public:
	FollowCamera() = default;
	~FollowCamera() = default;

	void ApplyGlobalVariables();
	void AddGlobalVariables() const;

	void Init();
	void Update();
	inline void SetTarget(const BaseTransform *const target) { target_ = target; }

	const Matrix4x4 &GetViewMatrix() const;
	const Matrix4x4 &GetProjectionMatrix() const;

	const auto *const GetCamera() const { return &camera_; }

	bool ImGuiWidget();
};
