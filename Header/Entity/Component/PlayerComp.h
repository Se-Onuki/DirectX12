#pragma once

#include "../Object.h"
#include <memory>
#include "../../../DirectBase/2D/Sprite.h"

#include "../../File/VariantItem.h"

class Input;
class ViewProjection;

class GameScene;

class ModelComp;
class Rigidbody;
class HealthComp;

class FollowCamera;
class Targeting;

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() override = default;

	void Init() override;
	void Update() override;

	void ApplyVariables(const char *const groupName) override;
	void AddVariable(const char *const groupName) const override;

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
	Rigidbody *rigidbody_ = nullptr;
	HealthComp *healthComp_ = nullptr;

	Targeting *targeting_ = nullptr;

	Vector3 sightCentor_{};
	Vector3 target_{};

	// std::unique_ptr<Object> box_ = nullptr;

	// 円形サイト
	std::unique_ptr<Sprite> sight_ = nullptr;

	// 追尾レティクル
	std::unique_ptr<Sprite> reticle_ = nullptr;

	std::unique_ptr<Sprite> healthBarFrame_ = nullptr;
	std::unique_ptr<Sprite> healthBar_ = nullptr;

	VariantItem<Vector2> vBarCentor_{ "barCentor", {640.f, 720.f - 64.f} };
	Vector2 barFlameSize_;
	VariantItem<Vector2> vBarFlame_{ "barFlame", {8.f ,8.f} };

	VariantItem<Vector2> vBarScale_{ "barScale", {360.f,32.f} };

	float sightScale_ = 1.f;

	float scaleSub_ = 0.2f;
	float scaleHeal_ = 0.001f;

private:

	int32_t coolTime_ = 0u;

	int32_t jumpLimter_ = 0u;

private:

	static const char *const groupName_;

	VariantItem<float> maxHealth_{ "maxHealth", 30.f };

	VariantItem<float> vSpeedDown_{ "speedDown", 0.5f };
	VariantItem<float> vJumpDown_{ "jumpDown", 0.5f };


	VariantItem<float> colliderRadius_{ "colliderRadius", 2.f };

	VariantItem<float> defaultSightRadius_{ "defaultSightRadius", 150.f };

	VariantItem<int32_t> fireCoolTime_{ "fireCoolTime", 10 };

	VariantItem<Vector3> nozzle_{ "nozzle", Vector3{ 0.75f,2.f,0.f } };

	VariantItem<float> bulletSpeed_{ "bulletSpeed", 1.5f };
	VariantItem<float> moveSpeed_{ "moveSpeed", 75.f };
	VariantItem<float> friction_{ "friction", 0.95f };

	VariantItem<float> jumpStrength_{ "jumpStrength", 750.f };

	VariantItem<Vector2> sightSpeed_{ "sightSpeed",{ 15.f,7.5f } };

	VariantItem<float> cameraRotateSpeed_{ "cameraRotateSpeed", 0.1f };

	VariantItem<float> maxSightScale_{ "maxSightScale", 1.f };
	VariantItem<float> minSightScale_{ "minSightScale", 0.3f };
};