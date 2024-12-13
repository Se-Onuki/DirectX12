/// @file HealthComp.h
/// @brief 体力コンポーネント
/// @author ONUKI seiya
#pragma once
#include "../Entity.h"

class HealthComp :public IComponent {
public:
	using IComponent::IComponent;
	~HealthComp() override = default;

	void Reset() override;

	void Update() override;
	/// @brief 最大体力の設定
	void SetMaxHealth(const float value);
	/// @brief 最大体力の取得
	float GetMaxHealth() const { return maxHealth_; }
	/// @brief 体力の加算
	void AddHealth(const float value);
	/// @brief 現在の体力の取得
	float GetNowHealth() const { return nowHealth_; }

	/// @brief 比率の取得
	/// @return 最大の場合1.fが返る
	float GetProgress() const { return nowHealth_ / maxHealth_; }

private:

	float maxHealth_;
	float nowHealth_;

};