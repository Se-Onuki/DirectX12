#pragma once
#include "../Object.h"
#include "../../File/GlobalVariables.h"
#include "../../../DirectBase/2D/Sprite.h"
#include <optional>

class GameScene;
class HealthComp;

class BossComp : public IComponent {
public:

	enum class Behavior {
		kMove,   // 移動
		kAttack, // 連続射撃
		kRange,	 // 範囲射撃
	};

	Behavior behavior_ = Behavior::kAttack;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;


	using IComponent::IComponent;
	~BossComp() override = default;

	void Init() override;

	void Update() override;

	void DrawUI() const;


	/// @brief jsonからの読み込み
	/// @param groupName グループ名
	virtual void ApplyVariables(const char *const groupName) override;

	/// @brief jsonへの紐づけ
	/// @param groupName グループ名
	virtual void AddVariable(const char *const groupName) const override;

	void SetGameScene(GameScene *const gameScene);

private:

	void CoolTimeUpdate();
	void AddCoolTime(uint32_t time) { coolTime_ += time; }

	void MoveBehavior();
	void AttackBehavior();

	void Attack();
	void AttackRange();

	void FireBullet(const Vector3 &spawnPos, const Vector3 &velocity, const int32_t lifeTime);

private:

	static const char *const groupName_;

	GameScene *gameScene_;

	HealthComp *healthComp_ = nullptr;

	std::unique_ptr<Sprite> bossText_ = nullptr;
	std::unique_ptr<Sprite> healthBarFrame_ = nullptr;
	std::unique_ptr<Sprite> healthBar_ = nullptr;

	Vector3 target_;
	int32_t coolTime_ = 120;

	VariantItem<int32_t> vBulletLifeTime_{ "bulletLifeTime", 60 };

	VariantItem<int32_t> vRangeLifeTime_{ "rangeLifeTime", 45 };
	VariantItem<int32_t> vRangeCoolTime_{ "rangeCoolTime", 90 };

	VariantItem<int32_t> vRangeBulletCount_{ "rangeBulletCount", 32 };
	VariantItem<float> vRangeAngle_{ "rangeAngle", 45.f };
	VariantItem<float> vRangeBulletSpeed_{ "rangeBulletSpeed", 3.5f };

	VariantItem<int32_t> vFireCoolTime_{ "fireCoolTime", 10 };
	VariantItem<float> vBulletSpeed_{ "bulletSpeed", 2.5f };
	VariantItem<Vector3> vNozzle_{ "nozzle", Vector3{0.f,3.f,0.f} };

	VariantItem<Vector2> vBarCentor_{ "barCentor", {640.f,180.f} };
	Vector2 barFlameSize_;
	VariantItem<Vector2> vBarFlame_{ "barFlame", {8.f ,8.f} };

	VariantItem<Vector2> vBarScale_{ "barScale", {360.f,50.f} };

	VariantItem<float> vRadius_{ "radius", 3.f };
	VariantItem<float> vMaxHealth_{ "maxHealth", 100.f };

};