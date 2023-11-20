#pragma once
#include "../Entity.h"
#include "../../../Engine/DirectBase/Input/Input.h"
#include "../../Collision/Collision.h"

#include "../../../Engine/DirectBase/File/VariantItem.h"
#include "FollowCameraComp.h"
#include "../../../Engine/DirectBase/Model/Model.h"
#include "PlayerAnimations/AnimationManager.h"
#include "PlayerState/IPlayerState.h"
#include "../../../Utils/Math/Vector3.h"
#include <memory>
#include "PlayerState/IPlayerState.h"
#include "PlayerAnimComp.h"

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() = default;

	void Init() override;

	void Reset() override;

	void Update() override;

	void Draw(const Camera3D &camera) const override;

	void ImGuiWidget() override;

	void ApplyVariables(const char *const groupName) override;
	void AddVariable(const char *const groupName) const override;
	void SetFollowCamera(FollowCameraComp *const followCamera) { pFollowCamera_ = followCamera; }

	VariantItem<float> vMoveSpeed_{ "MoveSpeed", 1000.f };
	VariantItem<float> vJumpPower_{ "JumpPower", 5.f };
	VariantItem<float> vJumpSpeed_{ "JumpSpeed", 0.2f };
	VariantItem<float> vJumpDeceleration_{ "JumpDeceleration", 0.95f };

	template<IsBasedIPlayerState State>
	void ChangeState() {  // 状態を変更するメソッド
		nextState_ = std::make_unique<State>(this);
	}

	template<IsBasedIPlayerState State>
	void SetState() {  // 状態を変更するメソッド
		nowState_ = std::make_unique<State>(this);
	}

	/// @brief 移動方向を入力
	/// @param vec 移動ベクトル
	void MoveInput(const Vector3 &vec);

	/// @brief ジャンプコマンド
	void JumpInput();

	/// @brief 着地しているか
	/// @return 着地したなら true
	bool GetIsLanding() { return isLanding_; }

private:

	Vector3 CalcMoveCollision();

private:

	FollowCameraComp *pFollowCamera_ = nullptr;

	static const std::string groupName_;

	// 所属するグループ
	int32_t registeredGroups_ = -1;

	AABB collider_;

	Input *input_;

	PlayerAnimComp *animationComp_ = nullptr;

	std::unique_ptr<IPlayerStateClass> nowState_ = nullptr;
	std::unique_ptr<IPlayerStateClass> nextState_ = nullptr;

	Material backMaterial_;

	bool isLanding_ = false;
};