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

	void ImGuiWidget() override;

	void SetViewProjection(const ViewProjection *const vp) { viewProjection_ = vp; }

	void SetGameScene(GameScene *const gameScene) { gameScene_ = gameScene; }

	void CoolTimeUpdate();
	void AddCoolTime(const uint32_t time) { coolTime_ += time; }

private:

	void Attack();

private:

	Input *input_ = nullptr;

	const ViewProjection *viewProjection_ = nullptr;
	GameScene *gameScene_ = nullptr;

	ModelComp *modelComp_ = nullptr;
	int32_t coolTime_ = 0u;

private:

	int32_t fireCoolTime_ = 10u;

	Vector3 nozzle_ = { 0.75f,2.f,0.f };

	float bulletSpeed_ = 1.5f;
	float moveSpeed_ = 0.025f;
	float friction_ = 0.95f;
};