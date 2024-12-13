/// @file FollowCameraComp.h
/// @brief フォローカメラのコンポーネント
/// @author ONUKI seiya
#pragma once
#include "../../../Engine/DirectBase/File/VariantItem.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include "../../Collision/Collision.h"
#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../Entity.h"

class FollowCameraComp : public IComponent
{
public:
	using IComponent::IComponent;
	~FollowCameraComp() = default;

	void Init() override;

	void Update() override;

	void ImGuiWidget() override;
	/// @brief 回転を追加
	void AddRotate(const Vector3 &euler);

	/// @brief ターゲットを設定
	void SetTarget(BaseTransform *const target);
	/// @brief ラインを設定
	void SetLine(const LineBase &target);
	/// @brief カメラを取得
	const auto &GetCamera() const { return camera_; }

	VariantItem<"LerpValue", float> vLerpValue{0.85f};

private:
	LineBase line_;

	BaseTransform *pTarget_ = nullptr;

	VariantItem<"Offset", Vector3> offset_{{0.f, 6.f, -30.f}};
	VariantItem<"AddOffset", Vector3> addOffset_{{0.f, 6.f, 0.f}};

	Quaternion rotate_;
	Vector3 prePos_;

	Camera3D *camera_;
};