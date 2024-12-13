/// @file ProjectileComp.h
/// @brief 投擲物のコンポーネント
/// @author ONUKI seiya
#pragma once
#include "../Entity.h"

class ProjectileComp :public IComponent {
public:
	using IComponent::IComponent;
	~ProjectileComp() override = default;

	void OnCollision(GameObject *other) override;
	/// @brief ダメージを設定
	void SetDamage(const float value) { damage_ = value; }

private:

	float damage_ = 0.f;
	bool isHitDestroy_ = true;

};