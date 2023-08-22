#pragma once
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../Math/Math.hpp"
#include "../Math/Vector2.h"

struct Transform;
class Input;

class FollowCamera {
	ViewProjection viewProjection_;
	const Transform *target_ = nullptr;
	Vector3 rotate_ = {};
	Vector2 cameraRotSpeed_ = Vector2{ 1.f,3.f } *Angle::Dig2Rad;

	float minRotate_ = -20.f * Angle::Dig2Rad;

	Vector3 defaultOffset_{ 3.f, 5.f, -13.f };
	Input *input_ = nullptr;

public:
	FollowCamera() = default;
	~FollowCamera() = default;

	void Init();
	void Update();
	inline void SetTarget(const Transform *const target) {
		target_ = target;
	}

	const Matrix4x4 &GetViewMatrix() const;
	const Matrix4x4 &GetProjectionMatrix() const;

	bool ImGuiWidget();

	ViewProjection *const GetViewProjection();
};
