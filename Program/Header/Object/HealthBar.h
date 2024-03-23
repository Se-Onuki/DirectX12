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

	void SetPercent(const float t);

	void SetCentor(const Vector2 pos);

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
