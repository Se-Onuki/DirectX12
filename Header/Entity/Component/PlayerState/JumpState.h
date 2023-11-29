#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

class PlayerJumpState : public IPlayerStateClass {
public:

	using IPlayerStateClass::IPlayerStateClass;
	~PlayerJumpState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kJumpStart; }

	int32_t preAnimKey_{};

private:

	// 再生トリガー
	bool jumpSETrigger_ = false;

	// ジャンプSE
	static uint32_t jumpSE_;

};

