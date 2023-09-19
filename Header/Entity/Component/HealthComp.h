#pragma once
#include "../Entity.h"

class HealthComp :public IComponent {
public:
	using IComponent::IComponent;
	~HealthComp() override = default;

	void Reset() override;

	void Update() override;

	void SetMaxHealth(const float value);
	float GetMaxHealth() const { return maxHealth_; }

	void AddHealth(const float value);

	float GetNowHealth() const { return nowHealth_; }

	/// @brief 比率の取得
	/// @return 最大の場合1.fが返る
	float GetProgress() const { return nowHealth_ / maxHealth_; }

private:

	float maxHealth_;
	float nowHealth_;

};