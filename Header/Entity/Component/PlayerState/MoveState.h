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

private:

	// 再生トリガー
	bool step1Trigger_ = false;
	bool step2Trigger_ = false;

	// ループ対応用トリガー
	bool canPlay_ = false;

	// 足音SE
	static uint32_t stepSE1_;
	static uint32_t stepSE2_;
	
};
