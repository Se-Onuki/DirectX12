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

	void AddRotate(const Vector3 &angle);

private:

	Vector4 startColor_{};
	Vector4 endColor_{};
	SoLib::DeltaTimer deltaTimer_{};

	Vector3 neutralAngle_{};

	Vector3 addAngle_{};

	float rotateFacing_{};

	Model *model_ = nullptr;

	Camera3D *rotateCamera_ = nullptr;

	// 回転SE
	static uint32_t rotateSE_;
};

