#pragma once
#include "../../../../Engine/DirectBase/Render/Camera.h"
#include "../PlayerAnimations/AnimationManager.h"
#include "../../../../Engine/DirectBase/Input/Input.h"
#include "../PlayerAnimComp.h"
#include "../../../../Engine/DirectBase/Base/Audio.h"

class PlayerComp;

class IPlayerStateClass {
public:
	IPlayerStateClass(PlayerComp *ptr);

	virtual ~IPlayerStateClass() = default;

	virtual void Init();

	virtual void Exit() {}

	virtual void Update(float deltaTime);

	virtual void Draw(const Camera3D &camera) const;

	virtual PlayerBehavior GetState() = 0;

protected:

	Input *const input_ = nullptr;

	PlayerComp *const pPlayer_ = nullptr;

	PlayerAnimComp *const pAnimation_ = nullptr;

private:

};

template <typename T>
concept IsBasedIPlayerState = std::is_base_of_v<IPlayerStateClass, T>;

/// @brief プレイヤの状態
enum class PlayerStateEnum : uint32_t {
	// 地上
	kIdle,			// 待機状態
	kMove,			// 歩行状態
	// 空中関係
	kJumpStart,		// 踏み切り状態
	kHovering,		// 浮遊状態
	kLand,			// 着地状態
	// 大砲関係
	kRide,			// 大砲に騎乗
	kFire,			// 大砲から発射
	// ステージ操作関係
	kRotateStart,	// ステージの回転開始
	kRotating,		// ステージの回転操作
	kRotateEnd		// ステージの回転終了
};
