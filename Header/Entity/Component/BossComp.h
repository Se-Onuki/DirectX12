#pragma once
#include "../Object.h"
#include "../../File/GlobalVariables.h"
#include "../../../DirectBase/2D/Sprite.h"

class GameScene;

class BossComp : public IComponent {
public:
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

	static const char *const groupName_;

	GameScene *gameScene_;

	std::unique_ptr<Sprite> healthBarFrame_ = nullptr;
	std::unique_ptr<Sprite> healthBar_ = nullptr;

	Vector3 target_;

	VariantItem<Vector2> vBarCentor_{ "barCentor", {640.f,180.f} };
	VariantItem<Vector2> vBarScale_{ "barScale", {360.f,50.f} };

	VariantItem<float> vRadius_{ "radius", 3.f };
	VariantItem<float> vMaxHealth_{ "maxHealth", 100.f };

};