#pragma once
#include "../../../../Engine/DirectBase/Render/Camera.h"
#include "../PlayerAnimations/AnimationManager.h"

class PlayerComp;

class IPlayerStateClass {
public:
	IPlayerStateClass(PlayerComp *ptr) :pPlayer_(ptr) {}

	virtual ~IPlayerStateClass() = default;

	virtual void Init();

	virtual void Update(float deltaTime);

	virtual void Draw(const Camera3D &camera) const;

	virtual PlayerBehavior GetState() = 0;

protected:

	PlayerComp *const pPlayer_ = nullptr;

private:

};

template <typename T>
concept IsBasedIPlayerState = std::is_base_of_v<IPlayerStateClass, T>;

/// @brief プレイヤの状態
enum class PlayerStateEnum : uint32_t {
	kIdle,			// 待機状態
	kMove,			// 歩行状態
	kJumpStart,		// 踏み切り状態
	kHovering,		// 浮遊状態
	kLand,			// 着地状態
	kRide,			// 大砲騎乗
	kFire,			// 発射
	kRotateStart,	// 回転開始
	kRotating,		// 回転操作
	kRotateEnd		// 回転終了
};
