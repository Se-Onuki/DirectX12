/// @file FollowCamera.h
/// @brief フォローカメラ
/// @author ONUKI seiya
#pragma once
#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../../Engine/Utils/Math/Transform.h"

class Input;

class FollowCamera {
	Camera3D camera_;

	const Transform *target_ = nullptr;
	Vector3 rotate_ = {};
	const float cameraRotSpeed_ = 3._deg;
	Input *input_ = nullptr;

public:
	FollowCamera() = default;
	~FollowCamera() = default;

	void Init();
	void Update();
	/// @brief 目標を設定
	inline void SetTarget(const Transform *const target) { target_ = target; }

	/// @brief ビュー行列を取得
	const Matrix4x4 &GetViewMatrix() const;
	/// @brief プロジェクション行列を取得
	const Matrix4x4 &GetProjectionMatrix() const;

	/// @brief カメラを取得
	const auto *const GetCamera() const { return &camera_; }
};
