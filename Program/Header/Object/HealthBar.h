/// @file HealthBar.h
/// @brief 体力バー
/// @author ONUKI seiya
#pragma once
#include "../../Engine/DirectBase/2D/Sprite.h"
#include <memory>
#include <array>

class HealthBar {
public:
	HealthBar();
	~HealthBar() = default;

	void Init();

	void Update(const float deltaTime);

	void Draw();

	/// @brief 体力の割合を設定
	void SetPercent(const float t);

	/// @brief 中心座標を設定
	void SetCentor(const Vector2 pos);

	/// @brief サイズを設定
	void SetScale(const Vector2 radius);


private:

	VariantItem<"BarCentor", Vector2> vBarCentor_{ {640.f, 720.f - 64.f} };
	VariantItem<"BarFlame", Vector2> vBarFlame_{ {8.f ,8.f} };
	VariantItem<"BarScale", Vector2> vBarScale_{ {360.f,32.f} };

	Vector2 barFlameSize_;

	std::unique_ptr<Sprite> backGround_ = nullptr;
	std::unique_ptr<Sprite> gauge_ = nullptr;
	std::unique_ptr<Sprite> decrementGauge_ = nullptr;

};
