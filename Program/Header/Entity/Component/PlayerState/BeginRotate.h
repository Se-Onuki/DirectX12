#pragma once
#include "IPlayerState.h"
#include "../../../../Engine/DirectBase/Render/Camera.h"

class PlayerBeginRotateState : public IPlayerStateClass {
public:

	using IPlayerStateClass::IPlayerStateClass;
	~PlayerBeginRotateState() override = default;

	void Init() override;

	void Update(float deltaTime) override;

	void Draw(const Camera3D &camera) const override;

	PlayerBehavior GetState() override { return PlayerBehavior::kRotateStart; }

	Vector3 startPos_;

	SoLib::DeltaTimer timer_;

private:

	// 再生トリガー
	bool start1Trigger_ = false;
	bool start2Trigger_ = false;

	// 足音SE
	static uint32_t startRotateSE1_;
	static uint32_t startRotateSE2_;
};

