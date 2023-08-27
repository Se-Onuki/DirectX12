#pragma once
#include "../Object.h"

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

private:

	float maxHealth_;
	float nowHealth_;

};