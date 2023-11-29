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
#include "GoalAnimations/GoalAnimComp.h"

class InGameUIManager;

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

	VariantItem<float> vRotateBeginTime_{ "RotateBeginTime", 0.75f };
	VariantItem<float> vRotateEndTime_{ "RotateEndTime", 0.75f };

	VariantItem<float> vRotateHeight_{ "RotateHeight", 10.f };

	InGameUIManager *pInGameUI_ = nullptr;

	const Vector3 radius_{ 0.75f,0.99f,0.75f };

	template<IsBasedIPlayerState State>
	void ChangeState() {  // 状態を変更するメソッド
		nextState_ = std::make_unique<State>(this);
	}

	template<IsBasedIPlayerState State>
	void SetState() {  // 状態を変更するメソッド
		nowState_ = std::make_unique<State>(this);
	}

	void SetInGameUIManager(InGameUIManager *const inGameUI);

	/// @brief 現在の状態の取得
	/// @return 現在の状態
	const auto &GetState() const { return nowState_; }

	/// @brief 移動方向を入力
	/// @param vec 移動ベクトル
	void MoveInput(const Vector3 &vec);

	/// @brief ジャンプコマンド
	void JumpInput();

	/// @brief 着地しているか
	/// @return 着地したなら true
	bool GetIsLanding() { return isLanding_; }

	const Vector3 *const GetGroundPos()const {
		if (groundPos_.y >= 10000.f) { return nullptr; }
		return &groundPos_;
	}

	int32_t GetGroup() const { return registeredGroups_; }

	void SetIsActiveGravity(bool flag) { isActiveGravity_ = flag; }

	AABB GetCollider() const { return referenceCollider_.AddPos(transform_->GetGrobalPos()); }

	void SetIsGoaled(bool isGoaled) { isGoaled_ = isGoaled; }

	int32_t rotateCount_;

private:

	Vector3 CalcMoveCollision();

private:

	bool isActiveGravity_ = true;

	Vector3 groundPos_;

	FollowCameraComp *pFollowCamera_ = nullptr;

	static const std::string groupName_;

	// 所属するグループ
	int32_t registeredGroups_ = -1;

	AABB referenceCollider_;

	Input *input_;

	bool isGoaled_ = false;

	PlayerAnimComp *animationComp_ = nullptr;

	std::unique_ptr<IPlayerStateClass> nowState_ = nullptr;
	std::unique_ptr<IPlayerStateClass> nextState_ = nullptr;

	Material backMaterial_;

	bool isLanding_ = false;
};