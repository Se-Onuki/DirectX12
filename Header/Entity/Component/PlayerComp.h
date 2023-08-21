#pragma once

#include "../Object.h"

class Input;
class ViewProjection;

class GameScene;

class ModelComp;

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() override = default;

	void Init() override;
	void Update() override;

	void Attack();

	void SetViewProjection(const ViewProjection *const vp) { viewProjection_ = vp; }

	void SetGameScene(GameScene *const gameScene) { gameScene_ = gameScene; }

	void CoolTimeUpdate();
	void AddCoolTime(const uint32_t time) { coolTime_ += time; }

private:
	Input *input_ = nullptr;

	const ViewProjection *viewProjection_ = nullptr;
	GameScene *gameScene_ = nullptr;

	ModelComp *modelComp_ = nullptr;

	const uint32_t shotCoolTime_ = 10u;
	uint32_t coolTime_ = 0u;

	const float bulletSpeed_ = 0.75f;
	const float moveSpeed_ = 0.025f;
	const float friction_ = 0.95f;
};