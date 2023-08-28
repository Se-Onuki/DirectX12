#pragma once

#include "../Object.h"
#include <memory>
#include "../../../DirectBase/2D/Sprite.h"

class Input;
class ViewProjection;

class GameScene;

class ModelComp;

class FollowCamera;
class Targeting;

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() override = default;

	void Init() override;
	void Update() override;

	void ApplyClobalVariables();
	void AddValue();

	void DrawUI() const;

	void ImGuiWidget() override;

	void SetViewProjection(ViewProjection *const vp) { viewProjection_ = vp; }

	void SetGameScene(GameScene *const gameScene) { gameScene_ = gameScene; }

	void CoolTimeUpdate();
	void AddCoolTime(uint32_t time) { coolTime_ += time; }

	void SetFollowCamera(FollowCamera *const fCamera) { followCamera_ = fCamera; }

	float GetSightRadius() const { return defaultSightRadius_ * sightScale_; }

private:

	void Attack();

	void Jump();

	void UpdateUI();

private:

	Input *input_ = nullptr;

	ViewProjection *viewProjection_ = nullptr;
	GameScene *gameScene_ = nullptr;
	FollowCamera *followCamera_ = nullptr;

	ModelComp *modelComp_ = nullptr;

	Targeting *targeting_ = nullptr;

	Vector3 sightCentor_{};
	Vector3 target_{};

	// std::unique_ptr<Object> box_ = nullptr;

	// 円形サイト
	std::unique_ptr<Sprite> sight_ = nullptr;

	// 追尾レティクル
	std::unique_ptr<Sprite> reticle_ = nullptr;

	float defaultSightRadius_ = 150.f;

	float maxSightScale_ = 1.f;
	float sightScale_ = 1.f;
	float minSightScale_ = 0.3f;

	float scaleSub_ = 0.2f;
	float scaleHeal_ = 0.001f;

private:

	int32_t coolTime_ = 0u;

	int32_t jumpLimter_ = 0u;

private:

	static const std::string groupName;

	int32_t fireCoolTime_ = 10u;

	Vector3 nozzle_ = { 0.75f,2.f,0.f };

	float bulletSpeed_ = 1.5f;
	float moveSpeed_ = 75.f;
	float friction_ = 0.95f;

	float jumpStrength_ = 750.f;

	Vector2 sightSpeed_ = { 15.f,7.5f };

	float cameraRotateSpeed_ = 0.1f;
};