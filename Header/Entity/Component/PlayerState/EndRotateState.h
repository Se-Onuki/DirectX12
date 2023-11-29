#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

class PlayerEndRotateState : public IPlayerStateClass {
public:

	using IPlayerStateClass::IPlayerStateClass;
	~PlayerEndRotateState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kRotateEnd; }

	SoLib::DeltaTimer timer_;

private:

	// 再生トリガー
	bool end1Trigger_ = false;
	bool end2Trigger_ = false;

	// 足音SE
	static uint32_t endRotateSE1_;
	static uint32_t endRotateSE2_;
};

