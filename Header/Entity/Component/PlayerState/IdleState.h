#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

template<>
class PlayerState<PlayerStateEnum::kIdle> :public IPlayerStateClass {
public:

	PlayerState() = default;
	~PlayerState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kIdle; }

};
