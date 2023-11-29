#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

class PlayerRotatingState : public IPlayerStateClass {
public:

	using IPlayerStateClass::IPlayerStateClass;
	~PlayerRotatingState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kRotating; }

private:

	Vector4 startColor_{};
	Vector4 endColor_{};
	SoLib::DeltaTimer deltaTimer_{};

	Model *model_;

	Camera3D *rotateCamera_ = nullptr;

	// 回転SE
	static uint32_t rotateSE_;
};

