#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

class PlayerMoveState :public IPlayerStateClass {
public:

	using IPlayerStateClass::IPlayerStateClass;
	~PlayerMoveState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kMove; }

};
