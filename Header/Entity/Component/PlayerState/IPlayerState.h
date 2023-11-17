#pragma once
#include "../../../../Engine/DirectBase/Render/Camera.h"
#include "../PlayerAnimations/AnimationManager.h"

class IPlayerStateClass {
public:
	IPlayerStateClass() = default;
	virtual ~IPlayerStateClass() = default;

	virtual void Init();

	virtual void Update(float deltaTime);

	virtual void Draw(const Camera3D &camera) const;

	virtual PlayerBehavior GetState() = 0;

private:

};

/// @brief プレイヤの状態
enum class PlayerStateEnum : uint32_t {
	kIdle,			// 待機状態
	kMove,			// 歩行状態
	kJumpStart,		// 踏み切り状態
	kHovering,		// 浮遊状態
	kLand,			// 着地状態
	kFire,			// 発射
	kRotateStart,	// 回転開始
	kRotating,		// 回転操作
	kRotateEnd		// 回転終了
};

template <PlayerStateEnum pState>
class PlayerState;
