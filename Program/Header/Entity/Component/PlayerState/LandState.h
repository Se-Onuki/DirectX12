#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

class PlayerLandState : public IPlayerStateClass {
public:

	using IPlayerStateClass::IPlayerStateClass;
	~PlayerLandState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kLand; }

private:

	// 再生トリガー
	bool landSETrigger_ = false;

	// 着地SE
	static uint32_t landSE_;

};

